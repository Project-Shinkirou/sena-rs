#include "../../common/console.h"
#include "../../common/audit.h"
#include "scene_gpu.h"
#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>
#include <switch.h>
#include <sys/stat.h>

static atomic_bool audio_running;
static void *audio_sink;
static int16_t pcm[2048] __attribute__((aligned(4096)));

/* Rust newlib std uses these hooks on Horizon. */
long getrandom(void *buffer, size_t length, unsigned int flags) {
  (void)flags;
  randomGet(buffer, length);
  return (long)length;
}
long sysconf(int name) {
  (void)name;
  return 4096;
}

static void audio_loop(void *unused) {
  (void)unused;
  AudioOutBuffer buffer = {
      .buffer = pcm, .buffer_size = sizeof(pcm), .data_size = 512 * 4};
  while (atomic_load(&audio_running)) {
    sena_audio_render_i16(audio_sink, pcm, 512);
    AudioOutBuffer *released = NULL;
    if (R_FAILED(audoutPlayBuffer(&buffer, &released)))
      break;
  }
  atomic_store(&audio_running, false);
}

int main(int argc, char **argv) {
  const char *root = argc > 1 ? argv[1] : "sdmc:/switch/sena/game";
  mkdir("sdmc:/switch/sena", 0777);
  freopen("sdmc:/switch/sena/player.log", "a", stderr);
  setvbuf(stderr, NULL, _IONBF, 0);
  fprintf(stderr, "sena_switch: root=%s argc=%d\n", root, argc);
  char boot_path[1024];
  snprintf(boot_path, sizeof(boot_path), "%s/data.pac", root);
  struct stat boot_stat;
  int boot_rc = stat(boot_path, &boot_stat);
  fprintf(stderr, "sena_switch: data.pac stat=%d mode=%o size=%lld\n",
          boot_rc, boot_rc == 0 ? (unsigned)boot_stat.st_mode : 0,
          boot_rc == 0 ? (long long)boot_stat.st_size : -1LL);
  bool gpu = !(argc > 2 && strcmp(argv[2], "--software") == 0);
  Framebuffer fb;
  if (gpu) {
    if (R_FAILED(romfsInit()) || !sena_scene_gpu_init()) {
      fputs("Native GPU initialization failed; use --software to select the "
            "CPU renderer.\n",
            stderr);
      return 1;
    }
  } else {
    Result rc = framebufferCreate(&fb, nwindowGetDefault(), 1280, 720,
                                  PIXEL_FORMAT_RGBA_8888, 2);
    if (R_FAILED(rc)) {
      fprintf(stderr, "framebufferCreate: %x\n", rc);
      return 1;
    }
    if (R_FAILED(framebufferMakeLinear(&fb))) {
      framebufferClose(&fb);
      return 1;
    }
  }
  padConfigureInput(1, HidNpadStyleSet_NpadStandard);
  PadState pad;
  padInitializeDefault(&pad);
  hidInitializeTouchScreen();
  void *host = sena_host_create(root, "shift-jis", "jp", 1280, 720);
  if (!host) {
    if (gpu) {
      sena_scene_gpu_exit();
      romfsExit();
    } else
      framebufferClose(&fb);
    return 1;
  }
  sena_host_set_scene_rendering(host, gpu);
  SenaAudit audit = sena_audit_open("sdmc:/switch/sena/audit.txt");
  unsigned frame = 0;
  audio_sink = sena_host_audio_sink(host);
  Thread audio_thread;
  bool audio_initialized = !(audit.active && audit.dt) && R_SUCCEEDED(audoutInitialize());
  if (audit.active)
    fputs(audit.dt ? "[audit] pcm=clocked-drain rate=48000\n"
                   : "[audit] pcm=realtime-worker rate=48000\n", stderr);
  bool audio_started = audio_initialized && R_SUCCEEDED(audoutStartAudioOut());
  bool thread_created = false, thread_started = false;
  if (audio_started) {
    atomic_store(&audio_running, true);
    thread_created = R_SUCCEEDED(threadCreate(&audio_thread, audio_loop, NULL,
                                              NULL, 128 * 1024, 0x2c, -2));
    thread_started = thread_created && R_SUCCEEDED(threadStart(&audio_thread));
  }
  SenaCursor cursor = {.x = 640, .y = 360};
  u64 previous = armGetSystemTick();
  while (appletMainLoop()) {
    u64 now = armGetSystemTick();
    uint32_t dt = (uint32_t)(armTicksToNs(now - previous) / 1000000);
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
    padUpdate(&pad);
    u64 buttons = padGetButtons(&pad);
    if ((buttons & (HidNpadButton_Plus | HidNpadButton_Minus)) ==
        (HidNpadButton_Plus | HidNpadButton_Minus))
      break;
    int lw = (int)sena_host_frame_width(host),
        lh = (int)sena_host_frame_height(host);
    SenaViewport v = sena_fit(1280, 720, lw, lh);
    HidAnalogStickState stick = padGetStickPos(&pad, 0);
    double dx = sena_axis(stick.x / 32767.0),
           dy = -sena_axis(stick.y / 32767.0);
    if (buttons & HidNpadButton_Left)
      dx = -1;
    if (buttons & HidNpadButton_Right)
      dx = 1;
    if (buttons & HidNpadButton_Up)
      dy = -1;
    if (buttons & HidNpadButton_Down)
      dy = 1;
    HidTouchScreenState touch = {0};
    hidGetTouchScreenStates(&touch, 1);
    if (audit.active)
      sena_audit_input(&audit, frame, host, &cursor, v, lw, lh);
    else
      sena_cursor_update(host, &cursor, v, lw, lh, dx * dt * 0.65, dy * dt * 0.65,
                       touch.count > 0, touch.touches[0].x, touch.touches[0].y,
                       buttons & HidNpadButton_A,
                       buttons & (HidNpadButton_B | HidNpadButton_Plus),
                       buttons & (HidNpadButton_R | HidNpadButton_ZR),
                       buttons & HidNpadButton_X);
    int status = sena_host_step(host, dt);
    ++frame;
    if (!thread_started || !atomic_load(&audio_running)) {
      int16_t silence[9600];
      sena_audio_render_i16(audio_sink, silence, dt * 48);
    }
    if (gpu) {
      sena_scene_gpu_render(host, &cursor);
    } else {
      uint32_t stride = 0;
      uint8_t *pixels = framebufferBegin(&fb, &stride);
      lw = (int)sena_host_frame_width(host);
      lh = (int)sena_host_frame_height(host);
      sena_blit(pixels, stride, 1280, 720, sena_host_frame_rgba(host), lw, lh,
                sena_fit(1280, 720, lw, lh), &cursor);
      framebufferEnd(&fb);
    }
    if (status != 0) {
      fprintf(stderr, "Engine stopped: %d\n", status);
      break;
    }
  }
  atomic_store(&audio_running, false);
  fprintf(stderr, "sena_switch: exit frame=%u\n", frame);
  if (audit.file)
    fclose(audit.file);
  if (thread_started)
    threadWaitForExit(&audio_thread);
  if (thread_created)
    threadClose(&audio_thread);
  if (audio_started)
    audoutStopAudioOut();
  if (audio_initialized)
    audoutExit();
  sena_audio_sink_destroy(audio_sink);
  sena_host_destroy(host);
  if (gpu) {
    sena_scene_gpu_exit();
    romfsExit();
  } else
    framebufferClose(&fb);
  return 0;
}
