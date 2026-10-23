# PlayStation Vita homebrew

Build with VitaSDK (including libvita2d and pthread), CMake, and Rust
`nightly-2026-01-20` with `rust-src`:

```sh
rustup toolchain install nightly-2026-01-20 --profile minimal --component rust-src
platform/vita/build_vita.sh
```

`VITASDK` defaults to `/usr/local/vitasdk`. `SENA_VITA_TOOLCHAIN` overrides the
Rust toolchain. The script builds the shared PAL core for
`armv7-sony-vita-newlibeabihf`, links the native host, and creates
`dist/vita/sena.vpk` with title ID `SENAPAL01`. SFO `ATTRIBUTE2 = 12` enables
extended memory for the 336 MiB combined CPU budget: a 48 MiB newlib arena and
up to 288 MiB of independently freed large Rust memory blocks, including their
page/header overhead. See [the allocation policy](../../reverse/adr/0011-separate-vita-large-allocations.md). The build disables SLP vectorization
as in siglus_rs to avoid Vita3K's affected 64-bit saturating NEON operations.

Install the VPK with your homebrew package manager, then copy the game root to
`ux0:data/sena/game/`. Logs go to `ux0:data/sena/player.log`. A launcher can
override the game root with its first application argument. The frontend defaults
to Shift-JIS resources and Japanese text.

| Input | Action |
|---|---|
| Left stick / D-pad | Move the pointer |
| Cross / front touchscreen | Left mouse button; advance, select, drag |
| Circle / Start | Right mouse button; script menu or cancel |
| R | Hold Control for skip |
| Square | Space; hide the text window where supported |
| Start + Select | Exit |

Ordinary frames now use vita2d/GXM textured quads directly, with cached
textures, tint, Z rotation, source-cell UVs, clipping, and letterboxing. vita2d
embeds precompiled GXP shaders: neither the build nor the player requires
`libshacccg.suprx`. PAL raster effects retain a per-frame software fallback.
Pass the game path followed by `--software` to force full CPU composition.
See [compiler/runtime dependency and validation](../../reverse/vita-precompiled-rendering.md).
AudioOut consumes 48 kHz stereo on a separate thread. Group-3 Ogg BGM uses a
bounded PCM queue, retaining encoded bytes rather than a whole decoded track;
MIX, WMA and movie soundtracks retain their separate decode paths. See
[the streaming decision](../../reverse/adr/0010-stream-vita-ogg-bgm.md). This package has been
compiled and executed locally in Vita3K. Execution and memory findings are recorded in the
[emulator report](../../reverse/console-emulator-validation.md). Hardware memory
use and frame rate remain unmeasured. Scene textures have an 80 MiB CDRAM and
8 MiB uncached-RAM budget, including retired allocations; excess frames use
CPU composition. The fallback framebuffer is allocated on demand. See
[shared limitations](../README.md) and [upstream provenance](../UPSTREAM.md).

```sh
docker build -t sena-vita-build platform/vita/docker
docker run --rm -v "$PWD:/src" sena-vita-build
```

For repeatable diagnostics, [create a portable host checkpoint](../scripts/create_checkpoint.py)
in a writable isolated game root using a host-audio build. Copy its `saveNNN.dat`
to the console game's `save/` directory. An optional `ux0:data/sena/audit.txt`
starts with `timestep_ms,frame_limit,checkpoint_slot`, followed by
`frame,x,y,left,skip` input rows in logical PAL pixels. The checkpoint loads before
the first frame; remove or rename the audit file to restore normal play. Capture
during full movies or unresolved sprite-effect waits is rejected. This diagnostic
format does not imply original-engine save interchange or arbitrary decoder-state
serialization. See [ADR-0012](../../reverse/adr/0012-host-diagnostic-checkpoints.md).
