# Reviewed native PAL contracts

`pal32.json` and the included animation, input, font, graphics, task, and
save manifests are the reviewed declaration sources. `generate.py` emits
`pal32.h` and `crates/pal-vm/src/abi/generated.rs`; it verifies evidence hashes
and addresses without inferring prototypes from IDA output.

There are 148 distinct declaration records. The generated per-build inventory
is authoritative for build-specific counts. These declarations are separate
from the 921 exported-name/build records whose boundary disassembly has been
reviewed across three binaries. Neither count establishes complete behavior.

Contracts describe raw argument widths, borrowed buffers, ownership, state
transitions, failure results, and version differences. Shared layouts include
the 24-byte animation frame, button reaction, transition, four-word sound range,
18-byte packed WAVEFORMATEX, and Koikake's 76-byte sprite save record. Later
sprite-info layouts must not use the Koikake type.

Incidental EAX values remain unresolved rather than receiving a guessed source
return type. Examples include `PalRandomReset`, target-list mutation, and
`PalSoundSetReserved`. Portable adapters can perform their proven side effects
without claiming a complete native callable prototype.

## Adding or changing an implementation

1. Check the binary body and callers: stack cleanup, argument order and width,
   signed comparisons, pointer accesses, output writes, and return paths. Record
   the binary SHA-256, function address, and checked evidence. An IDA prototype
   alone is insufficient; release and debug DLL addresses are not interchangeable.
2. Add the function definition to `pal32.json` before, or in the same change as,
   its implementation. Document ownership, nullability, failure results, and
   version differences in `notes`. If any ABI fact is uncertain, list it under
   `unresolved` and retain that uncertainty in the implementation notes.
3. Define recovered shared value layouts in `crates/pal-vm/src/abi/mod.rs` with
   `repr(C)` and compile-time size, alignment, and field-offset checks. Update the
   corresponding C layout in the generator. Native pointers must not appear in
   serialized portable state. Opaque native objects remain opaque.
4. Map the export to its portable implementation, then implement its adapter
   without narrowing arguments. Keep original failure behavior documented even
   when a portable adapter translates it to `bool`, `Option`, or `Result`.
5. Regenerate declarations, run the freshness check, and compile the native
   declarations for Windows x86. Validate affected behavior with isolated game
   runs. The current user instruction excludes adding regression tests.

## Interface boundaries

The native declarations describe **Win32 x86 cdecl**, with four-byte pointers
and four-byte stack slots, including IEEE-754 float arguments. The header rejects other targets; Rust function pointer
types live under `abi::win32` only on that target. All targets can read the
`abi::CONTRACTS` inventory and use the pointer-free shared layouts.

`PalSpriteOpaque *` is a native address, not a portable `SpriteHandle` arena key.
`PalRect32` is the same actual type used by the portable `PalRect` alias, and
`PalRasterSource32` is the actual type behind `fx::RasterSource`. This prevents
the renderer from maintaining a separate approximation of those value layouts.
Only the type-0 raster source is represented; other FX payloads require their own
verified layouts.

The Game.exe VM extcall interface is separately described by
`crates/pal-script/src/extsig.rs`. The newer registration-table profile has 69
individually reviewed wrapper contracts in `extsig_registered.rs`, generated
from the newer evidence; these override incompatible legacy names/arities. VM arguments can be handles, indices, or resource
names which the Game.exe wrapper resolves before calling PAL. Sena's
`crates/pal-vm/src/ffi.rs` and `platform/common/sena.h` define another interface:
the portable host API. Neither is a replacement for native DLL declarations.

For example, native `PalEffect` has two arguments. The portable effect method
additionally receives cached time from its host. Native `PalEffectEx` flags are
32-bit integers, even if an internal portable implementation uses booleans after
explicit conversion. Such context and conversions belong at the adapter boundary.

## First corrected mismatch

`PalSpriteRectSetPos` takes full 32-bit cell indices in both audited DLLs. It uses
wrapping 32-bit products and unsigned bounds comparisons; it does not narrow the
indices to 16 bits. The portable sprite API, script call, action scheduler, and
other callers now preserve that width. A native animation frame really is 16 bits
and is explicitly widened at the call boundary.
The Game.exe adapter is recorded separately in
`../evidence/pal-abi-vm-cell-wrapper.json`; it forwards both DWORDs unchanged.

Examples with a 64x32 texture and 16x16 cells: `(65536, -1)` selects
`(48, 16, 64, 32)`; `(0x10000001, 0x10000001)` wraps the products and selects
`(16, 16, 32, 32)`. An oversized cell also preserves native subtraction behavior
rather than saturating negative source coordinates to zero.

## Validation

```sh
python3 reverse/abi/generate.py
python3 reverse/abi/generate.py --check
i686-w64-mingw32-gcc -std=c11 -Wall -Wextra -Werror -fsyntax-only -x c reverse/abi/pal32.h
i686-w64-mingw32-g++ -std=c++11 -Wall -Wextra -Werror -fsyntax-only -x c++ reverse/abi/pal32.h
cargo check -p pal-vm --lib --no-default-features --target i686-pc-windows-gnu
cargo check -p pal-vm --lib --features host-audio
```

The release workflow checks generated declarations for drift. The generator also
checks each function's recorded address and binary hash against its evidence file.
CI does not need the game binaries or an IDA installation for this check.

## Newer cell and audio differences

Totsulover's `PalSpriteRectSetPos` compares signed cell indices against the
dimension/cell quotient, preserves negative indices, and special-cases a
one-pixel remainder. The old unsigned-product path remains for the earlier
profiles. `PalEffectEx` accepts IDs below 100 in this DLL.

`PalSoundLoadEx` takes a float gain as its fourth stack word and clips amplified
signed 16-bit PCM at load time. It is not a frequency parameter. `PalSoundPlay`
takes two cdecl stack words; the extra EDI argument in the decompiler output is
an analysis artifact from an indirect COM call. `PalSoundSetVolume` takes
volume before group, while channel setters take handle before value.
