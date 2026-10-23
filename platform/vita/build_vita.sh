#!/usr/bin/env bash
set -euo pipefail
ROOT=$(cd "$(dirname "$0")/../.." && pwd)
export VITASDK=${VITASDK:-/usr/local/vitasdk}
export PATH="$VITASDK/bin:$PATH"
TOOLCHAIN=${SENA_VITA_TOOLCHAIN:-nightly-2026-01-20}
BUILD="$ROOT/target/vita-native"
export CARGO_TARGET_ARMV7_SONY_VITA_NEWLIBEABIHF_LINKER=arm-vita-eabi-gcc
# Match siglus_rs: Vita3K mishandles the SLP-generated saturating 64-bit NEON ops.
export RUSTFLAGS="${RUSTFLAGS:-} -C no-vectorize-slp"
rustup run "$TOOLCHAIN" cargo build --manifest-path "$ROOT/platform/vita/core/Cargo.toml" \
    --release --target armv7-sony-vita-newlibeabihf --target-dir "$BUILD/rust" \
    -Zbuild-std=std,panic_abort
cmake -S "$ROOT/platform/vita" -B "$BUILD/cmake" \
    -DSENA_RUST_LIBRARY="$BUILD/rust/armv7-sony-vita-newlibeabihf/release/libsena_console.a"
cmake --build "$BUILD/cmake" --parallel
mkdir -p "$ROOT/dist/vita"
cp "$BUILD/cmake/sena.vpk" "$ROOT/dist/vita/sena.vpk"
printf 'Built %s\n' "$ROOT/dist/vita/sena.vpk"
