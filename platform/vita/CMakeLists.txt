cmake_minimum_required(VERSION 3.16)
if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VITASDK}/share/vita.toolchain.cmake" CACHE PATH "Vita toolchain")
endif()
project(sena_vita C)
include("$ENV{VITASDK}/share/vita.cmake" REQUIRED)
if(NOT DEFINED SENA_RUST_LIBRARY)
    message(FATAL_ERROR "SENA_RUST_LIBRARY must point to the compiled libsena_console.a")
endif()
add_executable(sena_vita source/main.c source/scene_gpu.c source/cpu_memory.c)
target_compile_options(sena_vita PRIVATE -O2 -Wall -Wextra -std=gnu11)
target_link_options(sena_vita PRIVATE -Wl,--gc-sections -Wl,--defsym=__sce_headroom=0x4000)
target_link_options(sena_vita PRIVATE -Wl,--wrap=malloc,--wrap=calloc,--wrap=realloc,--wrap=abort)
target_link_libraries(sena_vita PRIVATE "${SENA_RUST_LIBRARY}" vita2d
    SceGxm_stub SceDisplay_stub SceSysmodule_stub SceCommonDialog_stub
    SceAppUtil_stub SceAppMgr_stub SceAudio_stub SceCtrl_stub SceTouch_stub
    SceSysmem_stub ScePower_stub m pthread)
vita_create_self(eboot.bin sena_vita UNSAFE)
# Extended memory mode is required by the 336 MiB combined CPU budget.
set(VITA_MKSFOEX_FLAGS "-d ATTRIBUTE2=12")
vita_create_vpk(sena.vpk "SENAPAL01" eboot.bin VERSION "01.00" NAME "Sena")
