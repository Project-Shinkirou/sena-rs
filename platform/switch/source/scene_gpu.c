#include "scene_gpu.h"
#include "gpu.h"
#include <deko3d.h>
#include <stdio.h>
#include <stdlib.h>

enum { MaxCached = 2048 };
typedef struct {
  uint64_t id, generation;
  uint32_t width, height;
  int32_t gpu;
  bool used, seen;
} CachedTexture;
typedef struct {
  float position[2], uv[2], color[4];
} Vertex;
static CachedTexture cache[MaxCached];
static uint32_t sprite_vs, sprite_fs, raster_vs, raster_fs;
static int32_t white;

void sena_switch_log_message(const char *text) { fputs(text, stderr); }

bool sena_scene_gpu_init(void) {
  sena_gpu_init();
  sprite_vs = sena_gpu_program("sprite_vsh");
  sprite_fs = sena_gpu_program("sprite_fsh");
  raster_vs = sena_gpu_program("raster_vsh");
  raster_fs = sena_gpu_program("raster_fsh");
  return sprite_vs && sprite_fs && raster_vs && raster_fs;
}

static CachedTexture *find_texture(uint64_t id) {
  for (int i = 0; i < MaxCached; ++i)
    if (cache[i].used && cache[i].id == id)
      return &cache[i];
  return NULL;
}

static void sync_textures(void *host) {
  for (int i = 0; i < MaxCached; ++i)
    cache[i].seen = false;
  size_t count = sena_host_scene_texture_count(host);
  for (size_t i = 0; i < count; ++i) {
    SenaSceneTexture src;
    if (!sena_host_scene_texture(host, i, &src) || !src.width || !src.height ||
        src.byte_len != (size_t)src.width * src.height * 4)
      continue;
    CachedTexture *dst = find_texture(src.id);
    if (!dst) {
      for (int j = 0; j < MaxCached; ++j) {
        if (!cache[j].used) {
          dst = &cache[j];
          break;
        }
      }
      if (!dst) {
        fputs("GPU texture cache full\n", stderr);
        continue;
      }
      *dst = (CachedTexture){.id = src.id, .gpu = -1, .used = true};
    }
    dst->seen = true;
    if (dst->gpu < 0 || dst->width != src.width || dst->height != src.height) {
      if (dst->gpu >= 0)
        sena_gpu_texture_destroy(dst->gpu);
      dst->gpu = sena_gpu_texture_create(src.width, src.height, 1, 0);
      dst->width = src.width;
      dst->height = src.height;
      dst->generation = ~src.generation;
    }
    if (dst->gpu >= 0 && dst->generation != src.generation) {
      sena_gpu_texture_upload(dst->gpu, 0, src.rgba, src.width, src.height);
      dst->generation = src.generation;
    }
  }
  for (int i = 0; i < MaxCached; ++i) {
    if (cache[i].used && !cache[i].seen) {
      sena_gpu_texture_destroy(cache[i].gpu);
      cache[i].used = false;
    }
  }
}

static void draw_quad(const SenaSceneDraw *src, int lw, int lh,
                      SenaViewport viewport) {
  CachedTexture *tex = src->texture_id ? find_texture(src->texture_id) : NULL;
  if (src->texture_id && (!tex || tex->gpu < 0))
    return;
  SenaGpuDraw draw = {0};
  bool raster = src->raster[3] != 0;
  draw.vertex_program = raster ? raster_vs : sprite_vs;
  draw.fragment_program = raster ? raster_fs : sprite_fs;
  draw.attribs[0] = (SenaGpuVertexAttrib){0, 2};
  draw.attribs[1] = (SenaGpuVertexAttrib){8, 2};
  draw.attribs[2] = (SenaGpuVertexAttrib){16, 4};
  draw.stride = sizeof(Vertex);
  draw.vertex_count = 6;
  draw.blend_enable = !src->blend_disabled;
  draw.color_op = draw.alpha_op = DkBlendOp_Add;
  draw.color_src = DkBlendFactor_SrcAlpha;
  draw.color_dst = draw.alpha_dst = DkBlendFactor_InvSrcAlpha;
  draw.alpha_src = DkBlendFactor_One;
  switch (src->render_mode) {
  case 2:
    draw.color_dst = DkBlendFactor_One;
    break;
  case 3: case 5:
    draw.color_src = DkBlendFactor_Zero;
    draw.color_dst = DkBlendFactor_SrcColor;
    break;
  case 4:
    draw.color_src = DkBlendFactor_Zero;
    draw.color_dst = DkBlendFactor_DstColor;
    break;
  case 6:
    draw.color_src = DkBlendFactor_InvDstColor;
    draw.color_dst = DkBlendFactor_One;
    break;
  case 7: case 8: case 9: case 10:
    draw.color_src = draw.color_dst = DkBlendFactor_One;
    draw.color_op = src->render_mode == 7 ? DkBlendOp_Min
        : src->render_mode == 8 ? DkBlendOp_Max
        : src->render_mode == 10 ? DkBlendOp_RevSub : DkBlendOp_Add;
    break;
  case 11:
    draw.blend_enable = 0;
    break;
  }
  draw.color_write_mask = DkColorMask_RGBA;
  draw.cull_mode = DkFace_None;
  draw.front_face = DkFrontFace_CCW;
  for (unsigned i = 0; i < SenaGpuMaxTextureUnits; ++i)
    draw.textures[i] = -1;
  draw.textures[0] = tex ? tex->gpu : white;
  float uv[4][2];
  memcpy(uv, src->uv, sizeof(uv));
  bool smooth = false;
  if (tex && src->smooth_upscale && !raster) {
    float dx = hypotf(src->quad[1][0] - src->quad[0][0],
                      src->quad[1][1] - src->quad[0][1]);
    float dy = hypotf(src->quad[3][0] - src->quad[0][0],
                      src->quad[3][1] - src->quad[0][1]);
    float du = src->uv[1][0] - src->uv[0][0],
          dv = src->uv[3][1] - src->uv[0][1];
    smooth = dx * viewport.w / lw > du * tex->width * 1.1f ||
             dy * viewport.h / lh > dv * tex->height * 1.1f;
    if (smooth) {
      float iu = fminf(0.5f / tex->width, du * 0.25f),
            iv = fminf(0.5f / tex->height, dv * 0.25f);
      uv[0][0] += iu;
      uv[3][0] += iu;
      uv[1][0] -= iu;
      uv[2][0] -= iu;
      uv[0][1] += iv;
      uv[1][1] += iv;
      uv[2][1] -= iv;
      uv[3][1] -= iv;
    }
  }
  draw.samplers[0] = smooth ? SenaGpuSampler_Linear : SenaGpuSampler_Point;
  float quad[4][2];
  for (int i = 0; i < 4; ++i) {
    quad[i][0] = src->quad[i][0] * 2 / lw - 1;
    quad[i][1] = 1 - src->quad[i][1] * 2 / lh;
  }
  float uniforms[12] = {src->raster[0], src->raster[1], src->raster[2], 0};
  for (int axis = 0; axis < 2; ++axis) {
    uniforms[4 + axis * 4] = (quad[1][axis] - quad[0][axis]) * 0.5f;
    uniforms[5 + axis * 4] = (quad[0][axis] - quad[3][axis]) * 0.5f;
    uniforms[6 + axis * 4] = (quad[0][axis] + quad[2][axis]) * 0.5f;
  }
  if (raster) {
    draw.vertex_uniforms[0] = draw.fragment_uniforms[0] =
        (SenaGpuUniform){uniforms, sizeof(uniforms)};
    const float local[4][2] = {{-1, 1}, {1, 1}, {1, -1}, {-1, -1}};
    memcpy(quad, local, sizeof(quad));
  }
  Vertex vertices[6];
  const int order[6] = {0, 1, 2, 0, 2, 3};
  for (int i = 0; i < 6; ++i) {
    memcpy(vertices[i].position, quad[order[i]], sizeof(vertices[i].position));
    memcpy(vertices[i].uv, uv[order[i]], sizeof(vertices[i].uv));
    memcpy(vertices[i].color, src->color, sizeof(vertices[i].color));
    if (src->texture_rgb || src->render_mode == 11)
      vertices[i].color[0] = vertices[i].color[1] = vertices[i].color[2] = 1;
  }
  draw.vertices = vertices;
  draw.viewport[0] = viewport.x;
  draw.viewport[1] = viewport.y;
  draw.viewport[2] = viewport.w;
  draw.viewport[3] = viewport.h;
  draw.scissor[0] = viewport.x;
  draw.scissor[1] = viewport.y;
  draw.scissor[2] = viewport.w;
  draw.scissor[3] = viewport.h;
  sena_gpu_draw(&draw);
}

static void solid(float x, float y, float w, float h, const float color[4],
                  SenaViewport v, int lw, int lh) {
  SenaSceneDraw draw = {
      .quad = {{x, y}, {x + w, y}, {x + w, y + h}, {x, y + h}}};
  memcpy(draw.color, color, sizeof(draw.color));
  draw_quad(&draw, lw, lh, v);
}

void sena_scene_gpu_render(void *host, const SenaCursor *cursor) {
  if (!sena_gpu_begin_frame())
    return;
  static bool initialized;
  if (!initialized) {
    const uint8_t rgba[4] = {255, 255, 255, 255};
    white = sena_gpu_texture_create(1, 1, 1, 0);
    sena_gpu_texture_upload(white, 0, rgba, 1, 1);
    initialized = true;
  }
  sync_textures(host);
  const float black[4] = {0, 0, 0, 1};
  sena_gpu_begin_pass(-1, black, false, -1);
  int lw = sena_host_frame_width(host), lh = sena_host_frame_height(host);
  if (lw <= 0 || lh <= 0) {
    sena_gpu_end_frame(true);
    return;
  }
  SenaViewport v = sena_fit(1280, 720, lw, lh);
  const float *clear = sena_host_scene_clear_color(host);
  if (clear)
    solid(0, 0, lw, lh, clear, v, lw, lh);
  for (size_t i = 0, n = sena_host_scene_draw_count(host); i < n; ++i) {
    SenaSceneDraw draw;
    if (sena_host_scene_draw(host, i, &draw))
      draw_quad(&draw, lw, lh, v);
  }
  const float cursor_color[4] = {1, 1, 1, 1};
  SenaViewport screen = {0, 0, 1280, 720};
  solid(cursor->x - 5, cursor->y, 11, 1, cursor_color, screen, 1280, 720);
  solid(cursor->x, cursor->y - 5, 1, 11, cursor_color, screen, 1280, 720);
  sena_gpu_end_frame(true);
}

void sena_scene_gpu_exit(void) { sena_gpu_exit(); }
