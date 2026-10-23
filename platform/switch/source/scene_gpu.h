#pragma once
#include "../../common/console.h"
bool sena_scene_gpu_init(void);
void sena_scene_gpu_render(void *host, const SenaCursor *cursor);
void sena_scene_gpu_exit(void);
