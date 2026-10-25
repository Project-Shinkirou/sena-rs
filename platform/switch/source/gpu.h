/* Adapted from xmoezzz/siglus_rs, commit 5f358ef; MPL-2.0. See platform/UPSTREAM.md. */
/* Low-level deko3d renderer, driven by scene_gpu.c through the shared PAL
 * scene ABI. Every draw carries its complete state. Enum values are
 * deko3d's own (deko3d.h). */
#pragma once

#include <stdbool.h>
#include <stdint.h>

enum {
    SenaGpuMaxTextureUnits = 8,
    SenaGpuMaxVertexAttribs = 16,
    SenaGpuMaxUniformBlocks = 2,
};

/* Texture flags. */
enum {
    SenaGpuTexture_RenderTarget = 1u << 0, /* with its own Z24S8 depth-stencil */
};

/* Samplers (clamp to edge). */
enum {
    SenaGpuSampler_Linear = 0,    /* linear, linear between mip levels */
    SenaGpuSampler_Point = 1,
    SenaGpuSamplerCount = 2,
};

typedef struct SenaGpuVertexAttrib {
    uint32_t offset;
    uint32_t components; /* 32-bit floats; 0: unused location */
} SenaGpuVertexAttrib;

typedef struct SenaGpuUniform {
    const void* data;
    uint32_t size;
} SenaGpuUniform;

typedef struct SenaGpuDraw {
    uint32_t vertex_program;
    uint32_t fragment_program;
    /* attribs[location] */
    SenaGpuVertexAttrib attribs[SenaGpuMaxVertexAttribs];
    uint32_t stride;
    const void* vertices;
    uint32_t vertex_count;

    /* Blending (DkBlendOp / DkBlendFactor); blend_enable 0 writes as is. */
    uint8_t blend_enable;
    uint8_t color_op, color_src, color_dst;
    uint8_t alpha_op, alpha_src, alpha_dst;
    uint8_t color_write_mask; /* DkColorMask */

    uint8_t depth_test, depth_write;
    uint8_t depth_compare; /* DkCompareOp */
    uint8_t stencil_enable;
    uint8_t stencil_compare, stencil_fail, stencil_depth_fail, stencil_pass; /* DkCompareOp, DkStencilOp */
    uint8_t stencil_ref;
    uint8_t cull_mode;  /* DkFace */
    uint8_t front_face; /* DkFrontFace */

    /* Texture id per unit (-1: none) and SenaGpuSampler_*. */
    int32_t textures[SenaGpuMaxTextureUnits];
    uint8_t samplers[SenaGpuMaxTextureUnits];

    SenaGpuUniform vertex_uniforms[SenaGpuMaxUniformBlocks];
    SenaGpuUniform fragment_uniforms[SenaGpuMaxUniformBlocks];

    float viewport[4]; /* x, y, width, height in target pixels */
    int32_t scissor[4]; /* x, y, width, height */
} SenaGpuDraw;

/* Setup and frames (main.c). */
void sena_gpu_init(void);
void sena_gpu_exit(void);

/* Rust side. */
uint32_t sena_gpu_display_width(void);
uint32_t sena_gpu_display_height(void);
uint32_t sena_gpu_program(const char* name);
int32_t sena_gpu_texture_create(uint32_t width, uint32_t height, uint32_t mip_levels, uint32_t flags);
void sena_gpu_texture_upload(int32_t id, uint32_t level, const uint8_t* rgba, uint32_t width, uint32_t height);
void sena_gpu_texture_destroy(int32_t id);
bool sena_gpu_texture_read(int32_t id, uint8_t* rgba);
bool sena_gpu_begin_frame(void);
void sena_gpu_begin_pass(int32_t target, const float* clear_color, bool clear_depth, int32_t clear_stencil);
void sena_gpu_clear_stencil(uint8_t value);
void sena_gpu_draw(const SenaGpuDraw* draw);
void sena_gpu_end_frame(bool present);
