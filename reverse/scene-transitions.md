# PalEffectEx scene transitions

Audited 2026-10-23 against both release DLLs. Source:
[`effect/scene_transition.rs`](../crates/pal-vm/src/effect/scene_transition.rs).
Evidence, hashes, callbacks, pixel kernels, and the sine table:
[`scene-effects-native.json`](evidence/scene-effects-native.json).
This replaces the former `effect_id % 4` overlay approximation.

| Entry | Koikake PAL | Reference PAL |
|---|---|---|
| PalEffectEx | `0x100B2480` | `0x100A8140` |
| Callback table | `0x1011A0C0` | `0x1010C5D0` |
| Maximum ID | 29 | 49 |
| Crossfade | `0x100B37D0` | `0x100A7F70` |
| Blur | `0x100B2BA0` | `0x100A7400` |
| Shrink crop | `0x100B3530` | `0x100A7D70` |
| White fade | `0x100B2DF0` | `0x100A7620` |
| Mosaic | `0x100B3060` | `0x100A7840` |
| Mask | `0x100B27E0` | `0x100A6F60` |

## Implemented callbacks

| ID | Behavior |
|---:|---|
| 0 | No-op |
| 1 | Old frame with byte-truncated `(1-t)*255` alpha |
| 2 | Repeated in-place five-point blur of the old frame, with the same fade |
| 3 | Centered shrinking source crop; native Direct3D destination y uses x |
| 4 | Null callback |
| 5 | Captured texture filled white, alpha increasing to 255 |
| 6 | Half-sine mosaic, switching to a captured new scene strictly after midpoint |
| 7..9 | Null callbacks |
| 10..maximum | `Mask/MaskNN.bmp`, with `NN = id - 10` |

The blur's left/top samples have already been modified in the same pass. Its
clockwise edge copy is preserved. Mosaic down/up sampling uses the native
seven-bit fixed-point step, not a box average. The checked-in sine table
contains the reference DLL's exact 257 float bit patterns.

Masks use the blue channel and the recovered 31-level alpha band. PAL does
not resize the mask. The port bounds mismatched resources safely. An
uncompressed BMP decoder handles indexed 1/4/8-bit and RGB 24/32-bit rows,
padding, orientation, palette bounds, and the non-alpha reserved byte.
The actual Koikake `mask.pac` fixture supplies indexed 1280 by 720 masks.

The argument is a click-cancellation flag, including negative nonzero values;
it is not a color. The terminal frame survives repeated clock ticks until
presented. Disabling effects also disables action-bank effects. Runtime effect
bounds follow the same inspected-build profile as the action scheduler.

## Rendering and limitations

Native Direct3D callbacks themselves lock textures and perform these pixel
operations on the CPU before drawing an ordinary textured quad. The port
likewise submits the resulting texture through wgpu, Switch deko3d, or Vita
GXM. These transitions do not require a new Vita shader compiler.

The port currently captures the last composed logical scene, rather than
reading the native framebuffer at the precise call site. Interruption and
multiple calls in one VM frame still require original-engine trace comparison.
Null callback completion and zero-duration behavior are not fully verified.
The reference sine bits are used for both builds; Koikake's generated table
has not been compared bit for bit. Profile detection remains heuristic.

Tests cover native alpha bands, in-place blur, fixed-step resampling, strict
midpoint switching, retained terminal frames, click cancellation, real archived
BMP masks, and decoder bounds. These are component tests, not an exhaustive
pixel comparison of every transition against the original executable.
