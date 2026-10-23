#include "../../common/console.h"
#include "../../common/audit.h"
#include "scene_gpu.h"
#include <psp2/audioout.h>
#include <psp2/ctrl.h>
#include <psp2/kernel/processmgr.h>
#include <psp2/kernel/threadmgr.h>
#include <psp2/touch.h>
#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <vita2d.h>

// Keep large Rust allocations outside newlib's non-moving contiguous arena.
unsigned int _newlib_heap_size_user = 48 * 1024 * 1024;
static atomic_bool audio_running;
static void *audio_sink;
static int audio_port = -1;
static uint64_t diagnostic_frame;
extern void *__real_malloc(size_t size);
extern void *__real_calloc(size_t count, size_t size);
extern void *__real_realloc(void *ptr, size_t size);
extern void __real_abort(void) __attribute__((noreturn));
void *__wrap_malloc(size_t size) {
  void *ptr = __real_malloc(size);
  if (!ptr && size)
    fprintf(stderr, "[allocator-null] malloc bytes=%u caller=%p\n",
            (unsigned)size, __builtin_return_address(0));
  return ptr;
}
void *__wrap_calloc(size_t count, size_t size) {
  void *ptr = __real_calloc(count, size);
  if (!ptr && count && size)
    fprintf(stderr, "[allocator-null] calloc count=%u size=%u caller=%p\n",
            (unsigned)count, (unsigned)size, __builtin_return_address(0));
  return ptr;
}
void *__wrap_realloc(void *old, size_t size) {
  void *ptr = __real_realloc(old, size);
  if (!ptr && size)
    fprintf(stderr, "[allocator-null] realloc bytes=%u caller=%p\n",
            (unsigned)size, __builtin_return_address(0));
  return ptr;
}
void __wrap_abort(void) {
  fprintf(stderr, "[abort] frame=%llu caller=%p\n",
          (unsigned long long)diagnostic_frame, __builtin_return_address(0));
  __real_abort();
}
extern void sena_vita_install_alloc_reporter(void);
void sena_vita_allocation_failed(size_t size, size_t align) {
  fprintf(stderr, "[oom] requested=%u alignment=%u\n", (unsigned)size, (unsigned)align);
}
void sena_vita_panic_report(const char *message, size_t length,
                            const char *file, size_t file_length, uint32_t line) {
  fprintf(stderr, "[panic] %.*s at %.*s:%u\n",
          (int)(length > 2048 ? 2048 : length), message,
          (int)(file_length > 512 ? 512 : file_length), file, (unsigned)line);
}
void sena_vita_image_allocation(size_t bytes) {
  fprintf(stderr, "[image-allocation] frame=%llu requested=%u\n",
          (unsigned long long)diagnostic_frame, (unsigned)bytes);
  sena_vita_scene_log_memory("before-image", diagnostic_frame);
}
static int audio_loop(unsigned int args, void *argp) {
  (void)args;
  (void)argp;
  int16_t samples[1024];
  while (atomic_load(&audio_running)) {
    sena_audio_render_i16(audio_sink, samples, 512);
    if (sceAudioOutOutput(audio_port, samples) < 0)
      break;
  }
  atomic_store(&audio_running, false);
  return 0;
}

int main(int argc, char **argv) {
  const char *root = argc > 1 ? argv[1] : "ux0:data/sena/game";
  mkdir("ux0:data/sena", 0777);
  freopen("ux0:data/sena/player.log", "a", stderr);
  setvbuf(stderr, NULL, _IONBF, 0);
  sena_vita_install_alloc_reporter();
  sena_vita_scene_log_memory("before-gpu", 0);
  if (vita2d_init() <= 0)
    return 1;
  vita2d_set_clear_color(0xff000000);
  sceCtrlSetSamplingMode(SCE_CTRL_MODE_ANALOG);
  sceTouchSetSamplingState(SCE_TOUCH_PORT_FRONT,
                           SCE_TOUCH_SAMPLING_STATE_START);
  SceTouchPanelInfo panel = {0};
  sceTouchGetPanelInfo(SCE_TOUCH_PORT_FRONT, &panel);
  sena_vita_scene_log_memory("before-engine", 0);
  void *host = sena_host_create(root, "shift-jis", "jp", 960, 544);
  sena_vita_scene_log_memory("after-engine", 0);
  if (!host) {
    vita2d_fini();
    return 1;
  }
  bool native_scene = !(argc > 2 && strcmp(argv[2], "--software") == 0);
  SenaAudit audit = sena_audit_open("ux0:data/sena/audit.txt");
  sena_host_set_scene_rendering(host, native_scene);
  if (audit.active && audit.restore_slot >= 0) {
    int restored = sena_host_load_checkpoint(host, audit.restore_slot);
    fprintf(stderr, "[checkpoint] slot=%d restored=%d\n", audit.restore_slot, restored);
    if (!restored) { sena_host_destroy(host); vita2d_fini(); return 1; }
  }
  audio_sink = sena_host_audio_sink(host);
  audio_port = (audit.active && audit.dt) ? -1 : sceAudioOutOpenPort(
      SCE_AUDIO_OUT_PORT_TYPE_MAIN, 512, 48000, SCE_AUDIO_OUT_MODE_STEREO);
  if (audit.active)
    fputs(audit.dt ? "[audit] pcm=clocked-drain rate=48000\n"
                   : "[audit] pcm=realtime-worker rate=48000\n", stderr);
  SceUID thread = -1;
  bool thread_started = false;
  if (audio_port >= 0) {
    atomic_store(&audio_running, true);
    thread = sceKernelCreateThread("sena-audio", audio_loop, 0x10000100,
                                   128 * 1024, 0, 0, NULL);
    thread_started = thread >= 0 && sceKernelStartThread(thread, 0, NULL) >= 0;
  }
  // Allocate the full-screen CPU fallback only when an effect needs it.
  vita2d_texture *texture = NULL;
  SenaCursor cursor = {.x = 480, .y = 272};
  uint64_t frame = 0;
  uint32_t previous_buttons = 0, previous_touches = 0;
  uint64_t previous = sceKernelGetProcessTimeWide();
  while (true) {
    uint64_t now = sceKernelGetProcessTimeWide();
    uint32_t dt = (uint32_t)((now - previous) / 1000);
    previous = now;
    if (dt < 1)
      dt = 1;
    if (dt > 100)
      dt = 100;
    if (audit.active) {
      if (audit.dt)
        dt = audit.dt;
      if (frame >= audit.limit)
        break;
    }
    SceCtrlData pad = {0};
    sceCtrlPeekBufferPositive(0, &pad, 1);
    uint32_t b = pad.buttons;
    if ((b & (SCE_CTRL_START | SCE_CTRL_SELECT)) ==
        (SCE_CTRL_START | SCE_CTRL_SELECT))
      break;
    int lw = (int)sena_host_frame_width(host),
        lh = (int)sena_host_frame_height(host);
    double dx = sena_axis(((double)pad.lx - 128) / 127),
           dy = sena_axis(((double)pad.ly - 128) / 127);
    if (b & SCE_CTRL_LEFT)
      dx = -1;
    if (b & SCE_CTRL_RIGHT)
      dx = 1;
    if (b & SCE_CTRL_UP)
      dy = -1;
    if (b & SCE_CTRL_DOWN)
      dy = 1;
    SceTouchData touch = {0};
    sceTouchPeek(SCE_TOUCH_PORT_FRONT, &touch, 1);
    if (b != previous_buttons || touch.reportNum != previous_touches) {
      fprintf(stderr, "[input] frame=%llu buttons=%08x touches=%u cursor=%.1f,%.1f\n",
              (unsigned long long)frame, b, touch.reportNum, cursor.x, cursor.y);
      previous_buttons = b;
      previous_touches = touch.reportNum;
    }
    double tx = 0, ty = 0;
    if (panel.maxAaX > panel.minAaX && panel.maxAaY > panel.minAaY) {
      tx = ((double)touch.report[0].x - panel.minAaX) * 960 /
           (panel.maxAaX - panel.minAaX);
      ty = ((double)touch.report[0].y - panel.minAaY) * 544 /
           (panel.maxAaY - panel.minAaY);
    }
    if (audit.active)
      sena_audit_input(&audit, (unsigned)frame, host, &cursor,
                       sena_fit(960, 544, lw, lh), lw, lh);
    else
      sena_cursor_update(host, &cursor, sena_fit(960, 544, lw, lh), lw, lh,
                       dx * dt * 0.5, dy * dt * 0.5, touch.reportNum > 0, tx,
                       ty, b & SCE_CTRL_CROSS,
                       b & (SCE_CTRL_CIRCLE | SCE_CTRL_START),
                       b & SCE_CTRL_RTRIGGER, b & SCE_CTRL_SQUARE);
    diagnostic_frame = frame;
    int status = sena_host_step(host, dt);
    if (!thread_started || !atomic_load(&audio_running)) {
      int16_t silence[9600];
      sena_audio_render_i16(audio_sink, silence, dt * 48);
    }
    vita2d_wait_rendering_done();
    if (!native_scene || !sena_vita_scene_render(host, &cursor)) {
      if (!texture) {
        texture = vita2d_create_empty_texture(960, 544);
        if (!texture) {
          sena_vita_scene_log_memory("fallback-allocation-failed", frame);
          break;
        }
        texture->gxm_rtgt = NULL;
        texture->depth_UID = 0;
      }
      const uint8_t *rgba = native_scene ? sena_host_scene_rasterize(host)
                                          : sena_host_frame_rgba(host);
      lw = (int)sena_host_frame_width(host);
      lh = (int)sena_host_frame_height(host);
      sena_blit(vita2d_texture_get_datap(texture),
                vita2d_texture_get_stride(texture), 960, 544, rgba, lw, lh,
                sena_fit(960, 544, lw, lh), &cursor);
      vita2d_start_drawing();
      vita2d_disable_clipping();
      vita2d_clear_screen();
      vita2d_draw_texture(texture, 0, 0);
      vita2d_end_drawing();
      vita2d_swap_buffers();
    }
    if (status != 0) {
      fprintf(stderr, "Engine stopped: %d\n", status);
      break;
    }
    if (++frame % 120 == 0)
      sena_vita_scene_log_memory("running", frame);
  }
  atomic_store(&audio_running, false);
  sena_vita_scene_log_memory("exit", frame);
  if (audit.file)
    fclose(audit.file);
  if (thread_started)
    sceKernelWaitThreadEnd(thread, NULL, NULL);
  if (thread >= 0)
    sceKernelDeleteThread(thread);
  if (audio_port >= 0)
    sceAudioOutReleasePort(audio_port);
  sena_audio_sink_destroy(audio_sink);
  sena_host_destroy(host);
  vita2d_wait_rendering_done();
  sena_vita_scene_free();
  if (texture)
    vita2d_free_texture(texture);
  vita2d_fini();
  sena_vita_scene_log_memory("after-release", frame);
  sceKernelExitProcess(0);
  return 0;
}
