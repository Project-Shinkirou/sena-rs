# Nintendo Switch homebrew

Build with devkitPro's devkitA64, libnx, deko3d, uam and switch-tools, plus Python 3, curl,
patch, and Rust `nightly-2026-01-20` with `rust-src`:

```sh
rustup toolchain install nightly-2026-01-20 --profile minimal --component rust-src
platform/switch/build_switch.sh
```

`DEVKITPRO` defaults to `/opt/devkitpro`; `DEVKITA64` and
`SENA_SWITCH_TOOLCHAIN` can override their defaults. The script downloads a
checksum-verified libc 0.2.178 into `target/switch-native`, applies the newlib
layout patch there and builds the shared PAL core. Naga translates the shared
ordinary/raster WGSL to GLSL; uam compiles four native DKSH programs embedded in
RomFS. The script links libnx/deko3d and creates `dist/switch/sena.nro`. It does
not patch the user's Cargo registry. The version must match the nightly's
build-std lockfile; the script rejects a mismatch before compiling.

Copy the NRO to the SD card's `switch/sena/` directory and the game root to
`switch/sena/game/`. Launch in application mode with sufficient memory; applet
mode is not a supported memory budget. A launcher can override the game path
using the first application argument. Logs go to `switch/sena/player.log`.
The frontend defaults to Shift-JIS resources and Japanese text.

| Input | Action |
|---|---|
| Left stick / D-pad | Move the pointer |
| A / touchscreen | Left mouse button; advance, select, drag |
| B / Plus | Right mouse button; script menu or cancel |
| R / ZR | Hold Control for skip |
| X | Space; hide the text window where supported |
| Plus + Minus | Exit |

Rendering defaults to a 1280x720 deko3d scene renderer with native ordinary
and type-0 raster shaders. Texture generations avoid unnecessary uploads;
quad placement, Z rotation, tint, and nearest/linear sampling come from the
shared C scene ABI. Global raster still captures its background prefix on the
CPU. To use the libnx framebuffer/software compositor, pass the game path as
the first argument and `--software` as the second. Touch and pointer input
preserve aspect ratio. Audio uses audout at 48 kHz stereo. The native shaders
and NRO build with the local SDK. Ryujinx boot, ADV rendering and finite input
replay are recorded in the [emulator report](../../reverse/console-emulator-validation.md).
Switch hardware behavior has not been measured.
See [shared limitations](../README.md) and [upstream provenance](../UPSTREAM.md).

The CI environment can also be built locally with Docker:

```sh
docker build -t sena-switch-build platform/switch/docker
docker run --rm -v "$PWD:/src" sena-switch-build
```
