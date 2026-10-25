# VM dispatch and raster FX implementation

Vita follow-up: [native drawing with precompiled programs](vita-precompiled-rendering.md).
Ordinary Vita draws now use GXM; the compiler-module limitation below applies
only to rebuilding custom shaders, not the native graphics API or player.

Implemented 2026-10-23 after the [pre-fix audit](vm-pal-shader-audit.md).
This report describes actual runtime changes and separates tested behavior from
remaining native compatibility work. Game installations were read-only;
real-game checks used an isolated resource root with a separate save directory.

## VM dispatch

The reference Game.exe primary table at `0x4BB1D0` and extcall category pointers
at `0x49C360` share storage. The generator verifies equality of table addresses,
then emits aliases only for nonzero entries in the audited range. This replaces
the earlier assumption that high primary opcodes were just a name catalog.

- All 248 nonzero entries in slots 0..262 now have dispatch routes: 33 core
  branches and 215 aliases to extcall handlers. Fifteen entries are native nulls.
- Aliased direct instructions advance four bytes and do not read or write an
  extcall destination. Suspended commands rewind by their actual instruction
  size, including select continuation. The ordinary opcode-23 form remains
  twelve bytes with its existing result operand.
- Primary 252 consumes two operands and 253 consumes one. They use the shared
  timed/click-wait behavior, including the indefinite sentinel. This corrects
  the former unconditional one-frame/no-operand and click/no-operand branches.
- Known signatures with no runtime implementation now reach `ExtCallSkipped`
  diagnostics instead of silently returning zero through a separate fallback.

This is dispatch coverage of one native table range, **not complete semantics
for 248 handlers**. The existing wait scheduler, overflow behavior, task
continuations, and partially implemented extcalls still need further comparison.
The signature registry now has 358 unique coordinates: 151 Verified, 38 Partial,
167 Blocked, and 2 StackDisciplineOnly. These are signature evidence statuses,
not a percentage of implemented engine behavior.

Sources: [alias generator](evidence/generate_primary_aliases.py),
[verified mappings](evidence/primary-extcall-aliases.json),
[coverage inventory](evidence/primary-opcode-coverage.json), and
[effective signature catalog](EXTCALL_CATALOG.md).

## Recovered type-0 raster lifecycle

The fixture Pal.dll allocator at `0x100B3E30` copies a twelve-byte source block:
`{ u32 period_ms; float undulate_x; float undulate_y; }`. The native descriptor
table at `0x1011A140` contains the type-0 software/hardware callbacks and twelve
bytes each for source and work data. Only this evidenced FX type is accepted.

The implementation in `crates/pal-vm/src/fx.rs` follows these native rules:

- Start enters state 1, ramps amplitude from zero over the requested duration,
  then holds at full strength until stopped or cleared.
- Stop (`0x100B3F20`) enters state 2 and ramps from full amplitude to zero, even
  if called during a partially completed entry ramp. Completion clears state.
- Update (`0x100B3FE0`) uses the wrapping millisecond clock. The wave phase
  (`0x100B4180`) shares an origin across instances: when elapsed reaches the
  period, it resets the origin and emits 1.0; it is not a modulo operation.
- Game category 22 slots 4..7 (also category 19 slots 0..3 in the same table)
  now clear, stop, wait for, and start sprite FX. Start consumes slot, horizontal
  percent, vertical percent, and duration; duration is also the wave period.
  Native unsigned percent conversion is retained.
- Global raster (`16:5`, Game `0x411760`) captures commands behind its insertion
  depth. Foreground UI remains outside the effect. `effect_stop` (`16:0`,
  `0x412450`) with flag `0x10` starts its ending ramp.

Evidence: [ramp update](evidence/runtime-audit/fx_fixture_100b3fe0.c),
[phase/amplitudes](evidence/runtime-audit/fx_fixture_100b4180.c), and the
Game [sprite start](evidence/runtime-audit/fx_game_4224a0.c),
[sprite wait](evidence/runtime-audit/fx_game_422790.c), and
[global raster](evidence/runtime-audit/fx_game_411760.c). Allocator/stop evidence
is linked by the earlier audit. Fixture and reference
addresses are deliberately kept separate.

## Shader math and rendering

[The disassembler](evidence/disassemble_fx.py) decodes the D3D9 instruction
streams instead of treating similarly named effects as interchangeable.
[JSON instructions/constants](evidence/fx-bytecode.json) and
[readable assembly](evidence/fx-disassembly.txt) preserve the evidence.

| Program | Current implementation and use |
|---|---|
| RASTER | Vertex and fragment stages rebuilt from `vs_1_1`/`ps_2_0` instructions. Native range reduction and cosine polynomial are preserved; amplitude does not change frequency. Sprite/global runtime FX reach the active wgpu raster pipeline and Switch deko3d shaders. |
| RASTER_SW | Removed vertex distortion copied from a commented-out HLSL block; kept active fragment expression and scalar Color semantics. Valid WGSL, but not selected by the runtime. The undefined amplitude-1 division is guarded for finite portable output. |
| RADIAL_BLUR | Corrected to the native absolute center, logarithmic divisor, texture dimensions, and twelve taps from -6 through +5. Valid WGSL; native effect dispatch and lifecycle are still not connected. |
| BICUBIC | Decoded coefficients confirm the cubic kernel. The native UV phase and pass selection remain unverified; not selected by the runtime. |
| DRAW_PRIMITIVE_UP2D | Used for ordinary Switch textured drawing. Desktop continues using its existing generic textured-quad program. |

`SpriteDraw` carries raster uniforms, and the CPU reference applies the same
local vertex transform and UV polynomial as the hardware RASTER program.
Z rotation now transforms the sampled quad in the software and GPU paths.
This does not implement X/Y rotation or all PAL render modes.

The wgpu path has a real second pipeline, uniform bindings, and per-draw shader
selection. The global raster prefix is currently **composited on the CPU**, then
its captured texture is distorted by the selected renderer. Per-sprite raster
runs directly on the GPU. The CPU reference emulates the hardware shader; it
does not claim bit equality with PAL's separate legacy software sine routine.

The native scene C ABI exports borrowed texture pixels, generations, quad
geometry, tint, and raster uniforms. Pixel storage remains valid until the next
step, mode change, or host destruction. Switch uses this mode and skips full
CPU frame composition for ordinary frames. PS Vita subsequently gained ordinary GPU drawing with a CPU effect fallback;
see the follow-up above.

## Console implementation

The Switch deko3d renderer and WGSL-to-GLSL compiler are adapted from
`siglus_rs` commit `5f358ef1360b0439e92aed2b749e8f25d8721954` under MPL-2.0.
The build compiles the shared ordinary/raster WGSL through Naga and uam into
four DKSH programs embedded in the NRO RomFS. The scene consumer maintains a
generational texture cache and binds native vertex/fragment uniforms, sampler,
source-over blend state, viewport, and controller cursor. `--software` remains
an explicit alternative. See [provenance](../platform/UPSTREAM.md).

PS Vita's rebuilt VPK supports raster through the CPU reference. Its native
ordinary renderer now uses vita2d's precompiled GXP programs without a compiler
module. Native PAL raster shader translation is still incomplete. The referenced
Cg rebuild tool needs `libshacccg.suprx`, which was not found locally; that is
not a dependency of GXM itself or the packaged player. No console hardware or emulator rendering run
has been performed for these packages.

## Validation

| Scope | Result |
|---|---|
| Default VM library | 151 passed, 5 ignored |
| Focused effect, engine, movie timing, MPEG, operand, runtime, sprite, system, task integration binaries | 85 passed |
| Shared script crate | 14 passed |
| Host-audio-only VM library | 150 passed, 4 ignored; alternate configuration, not added to the default count |
| Explicit ignored GPU readback test on Apple M4 Max / Metal | Passed; also validates all five WGSL modules |
| iOS arm64, Android arm64, Windows x64 GNU, WASM default-feature library checks | Passed |
| Switch native GPU NRO and Vita CPU VPK | Built with local SDKs |
| Shared console C input/letterbox tests | Passed with warnings as errors |

The real Metal test renders four raster phase/rotation cases and reads back the
actual target. Three match the CPU reference byte-for-byte; the rotated case
has mean absolute byte error `0.0008544921875`. This establishes agreement of
our CPU/GPU implementations, **not equivalence to native Windows captures**.

A release host-audio build ran 3,600 fixed 16 ms Koikake frames, clicked GAME
START, and advanced through ADV scenes. PCM was nonzero on 2,181 frames, peak
18,205, and the retained sink became silent after engine destruction. Final
images were inspected. A second 1,800-frame run exercised the native scene ABI,
validated draw/texture references and pixel lengths, and confirmed that scene
mode avoids the full CPU frame output. Neither run validates console hardware.

Logs and reports are under `target/reverse-audit`: `fx-regression.log`,
`fx-script-tests.log`, `fx-host-tests.log`, `fx-gpu.log`, `fx-ios-check.log`,
`fx-android-check.log`, `fx-windows-check.log`, `fx-wasm-check.log`,
`fx-switch-gpu-build.log`, `fx-vita-build.log`, `fx-ffi-smoke.log`,
`console-ffi-report.json`, and `fx-scene-ffi-report.json`.

## Remaining compatibility work

- Full `PalEffectEx` per-ID implementations and build-specific limits. The
  existing modulo-based overlay approximation remains and must not be counted
  as a completed transition implementation.
- Native effect dispatch for radial blur/bicubic, exact sampler/POT padding and
  clipping, all render modes, X/Y rotation, and visual action timelines.
- Serialization of active raster FX in portable snapshots; loading currently
  does not restore their timeline. Full native save-image interoperability is
  also incomplete.
- Native Windows frame comparisons and console runtime tests; GXM shader build
  and integration on Vita remain pending.
- Existing text/history, sound restoration, and other boundaries listed in
  [port corrections](port-corrections.md) still apply.
