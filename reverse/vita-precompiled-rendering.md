# Vita rendering with precompiled programs

Updated 2026-10-23. This corrects the dependency wording in the earlier
[FX implementation report](fx-vm-implementation.md).

## Compiler versus runtime

Using the native GXM API does not require `libshacccg.suprx`. GXM loads compiled
GXP programs. vita2d statically embeds its color, texture, and texture-tint GXP
programs; the installed VitaSDK archive contains those program symbols and no
Shacc compiler dependency. The upstream implementation binds these programs
through the shader patcher:
[libvita2d source](https://github.com/xerpi/libvita2d/blob/master/libvita2d/source/vita2d.c).

The pinned `siglus_rs` source also includes fifteen compiled programs in
`crates/siglus_scene_vm/src/render/vita/shaders/gxp/`. Its player embeds them
without requiring a shader compiler. Its separate `platform/vita/shaderc`
tool loads `libshacccg.suprx` to rebuild changed Cg sources. Thus the missing
local module prevents using that particular rebuild tool, not implementing or
running a native renderer. An already compiled compatible program can be reused.
The Siglus raster formula differs from PAL's; sharing a name is insufficient
justification to substitute it for the recovered PAL shader.

## Implemented host path

`platform/vita/source/scene_gpu.c` now submits ordinary PAL scene quads through
vita2d/GXM using the shared scene ABI. It uses vita2d's embedded programs and
does not import or load a shader compiler.

- Per-texture generations avoid unchanged uploads. Only referenced textures
  remain cached, and row copies honor vita2d's aligned texture stride.
- Native textured quads preserve source UVs, Z rotation, scale, tint/alpha,
  foreground order, clipping, and the same letterbox rectangle as input.
  Nearest/linear sampling and cell-edge UV inset follow the shared renderer.
- Changed textures receive fresh storage. Old allocations are retained for four
  frames, following the upstream Vita3K lifetime workaround. Small textures use
  uncached user memory instead of consuming a whole 256 KiB CDRAM allocation.
  Non-render-target fields are explicitly initialized before vita2d frees them.
- PAL raster draws still select the CPU reference for that frame. Unsupported
  image sizes or allocation failures also select the CPU path, without
  advancing the script again or dropping the frame's effect.
- `sena_host_scene_rasterize` lazily produces the current scene's RGBA buffer;
  ordinary GPU frames do not produce one. `--software` as the second argument
  forces the previous full CPU compositor path.

This is native GPU ordinary drawing with an explicit effect fallback. It is
not yet a native PAL raster GXP port. Other native PAL shader/effect gaps in the
previous report remain open.

## Validation

The VPK was rebuilt against the local VitaSDK. The linked ELF includes the new
scene consumer and vita2d's texture-tint GXP program. No Shacc compilation or
compiler-module installation was used.

Six focused FFI tests pass, including lazy CPU rendering that preserves frame
generation and scene views. A macOS release build exercises the same C scene
ABI with Koikake for 1,800 frames; four explicit CPU fallback requests are
cached and preserve VM frame generation. This does not execute the Vita GPU
consumer or certify its device pixels or frame rate.

Logs: `target/reverse-audit/vita-native-scene-build.log`,
`vita-scene-ffi-tests.log`, `vita-scene-ffi-smoke.log`, and
`vita-scene-ffi-report.json`. No Vita hardware or emulator rendering run is
claimed in this follow-up.
