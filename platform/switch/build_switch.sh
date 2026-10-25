#!/usr/bin/env bash
set -euo pipefail
ROOT=$(cd "$(dirname "$0")/../.." && pwd)
export DEVKITPRO=${DEVKITPRO:-/opt/devkitpro}
export DEVKITA64=${DEVKITA64:-$DEVKITPRO/devkitA64}
export PATH="$DEVKITA64/bin:$DEVKITPRO/tools/bin:$PATH"
BUILD="$ROOT/target/switch-native"
OUT="$ROOT/dist/switch"
TOOLCHAIN=${SENA_SWITCH_TOOLCHAIN:-nightly-2026-01-20}
# Match this nightly's build-std lockfile so std actually uses the layout patch.
LIBC_VERSION=0.2.178
LIBC_SHA=37c93d8daa9d8a012fd8ab92f088405fb202ea0b6ab73ee2482ae66af4f42091
SENA_STD_SYSROOT=$(rustup run "$TOOLCHAIN" rustc --print sysroot)
python3 - "$SENA_STD_SYSROOT/lib/rustlib/src/rust/library/Cargo.lock" "$LIBC_VERSION" <<'PY'
import pathlib, re, sys
versions = set(re.findall(r'^name = "libc"\nversion = "([^"]+)"',
                          pathlib.Path(sys.argv[1]).read_text(), re.M))
if versions != {sys.argv[2]}:
    raise SystemExit(f"build-std libc {versions} does not match the patched {sys.argv[2]}; update the pinned source and checksum together")
PY
mkdir -p "$BUILD" "$OUT"
if [[ ! -f "$BUILD/libc-$LIBC_VERSION/.patched" ]]; then
    curl -fL "https://static.crates.io/crates/libc/libc-$LIBC_VERSION.crate" -o "$BUILD/libc.crate"
    python3 - "$BUILD/libc.crate" "$LIBC_SHA" <<'PY'
import hashlib, pathlib, sys
assert hashlib.sha256(pathlib.Path(sys.argv[1]).read_bytes()).hexdigest() == sys.argv[2], "libc checksum mismatch"
PY
    tar -xzf "$BUILD/libc.crate" -C "$BUILD"
    (cd "$BUILD/libc-$LIBC_VERSION" && patch -p1 < "$ROOT/platform/switch/patches/libc-horizon-layout.patch")
    touch "$BUILD/libc-$LIBC_VERSION/.patched"
fi
rustup run "$TOOLCHAIN" cargo build --manifest-path "$ROOT/platform/switch/core/Cargo.toml" \
    --release --target "$ROOT/platform/switch/rust/aarch64-switch.json" \
    --target-dir "$BUILD/rust" -Zbuild-std=std,panic_abort \
    --config "patch.crates-io.libc.path=\"$BUILD/libc-$LIBC_VERSION\""
cargo run --release --manifest-path "$ROOT/platform/switch/shaderc/Cargo.toml" -- \
    "$ROOT/crates/pal-vm/src/renderer/shaders" "$BUILD/glsl"
mkdir -p "$BUILD/romfs/shaders"
for program in sprite raster; do
    uam --stage=vert --out="$BUILD/romfs/shaders/${program}_vsh.dksh" "$BUILD/glsl/${program}_vsh.glsl"
    uam --stage=frag --out="$BUILD/romfs/shaders/${program}_fsh.dksh" "$BUILD/glsl/${program}_fsh.glsl"
done
for source in main gpu scene_gpu; do
    aarch64-none-elf-gcc -std=gnu11 -O2 -Wall -Wextra -ffunction-sections -fdata-sections \
        -march=armv8-a+crc+crypto -mtune=cortex-a57 -mtp=soft -fPIE \
        -I"$DEVKITPRO/libnx/include" -c "$ROOT/platform/switch/source/$source.c" -o "$BUILD/$source.o"
done
aarch64-none-elf-gcc -specs="$DEVKITPRO/libnx/switch.specs" \
    -march=armv8-a+crc+crypto -mtp=soft -fPIE -Wl,--gc-sections \
    "$BUILD/main.o" "$BUILD/gpu.o" "$BUILD/scene_gpu.o" "$BUILD/rust/aarch64-switch/release/libsena_console.a" \
    -L"$DEVKITPRO/libnx/lib" -ldeko3d -lnx -lm -o "$BUILD/sena.elf"
nacptool --create "Sena" "sena_rs" "0.1.0" "$BUILD/sena.nacp"
elf2nro "$BUILD/sena.elf" "$OUT/sena.nro" --nacp="$BUILD/sena.nacp" --romfsdir="$BUILD/romfs"
printf 'Built %s\n' "$OUT/sena.nro"
