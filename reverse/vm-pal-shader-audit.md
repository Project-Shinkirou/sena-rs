# VM, PAL calls, and shader audit

Historical pre-fix snapshot, audited 2026-10-23 against the then-current tree,
local PE files, and fresh IDA 9.0 decompilation. The missing dispatch routes,
252/253 operands, raster lifecycle, and active shader integration below were
subsequently addressed in [VM dispatch and raster FX implementation](fx-vm-implementation.md).
Read that follow-up for current status and validation. The remaining tables in
this document preserve the original findings, not the post-fix runtime state.

## Build identity and counting

The game installation and the existing reverse database are different builds:

| Binary set | Executable | PAL PE exports | Executable PAL imports |
|---|---|---:|---:|
| Current local fixture | `testcase/koikake.exe` | 262 in `testcase/dll/Pal.dll` | 192 |
| Existing reverse reference | `reverse/Game.exe` | 290 in `reverse/PAL.dll` | 215 |
| Debug reference | n/a | 288 in `reverse/PAL_d.dll` | n/a |

These are nonzero PE export entries and PAL import entries, including delayed
imports. The older count of 289 for the debug DLL included IDA's synthetic
`DllEntryPoint` entry. The reference release has two additional exported names,
`PalSpriteGetPosX` and `PalSpriteGetPosY`. `PAL.sqlite` is an export of
**PAL_d.dll**, not the release DLL; its input MD5 matches that debug binary.
`Game.sqlite` matches `reverse/Game.exe`. The fixture DLL is a third build.
The fixture and reference `FX.dll` files are byte-identical.

[Native inventory](evidence/native-interface-inventory.json) records hashes,
all names/addresses, and the build comparison. Regenerate the PE inventory with:

```sh
python3 reverse/evidence/audit_native_interfaces.py
```

The Rust signature registry has 350 unique coordinates: 143 Verified, 38 Partial,
167 Blocked, and 2 StackDisciplineOnly. These statuses concern signatures and
recorded verification, not complete runtime semantics. They are neither the
number of PAL exports nor a completion percentage. See
[EXTCALL_CATALOG.md](EXTCALL_CATALOG.md).

## VM coverage

The reference executable's `VmScriptRunStep` at `0x44A2F0` accepts a word with
high half 1 and directly calls `gOpcodeDispatchTable[low16]`. There is no
33-opcode limit in this dispatch. Fresh IDA extraction of the known 0..262
range at `0x4BB1D0` found:

| Classification within this table range | Slots |
|---|---:|
| Native null entries | 15 |
| Core opcodes 1..33, with direct Rust branches | 33 |
| Opcodes 252/253, with Rust branches but mismatched contracts | 2 |
| Other nonzero entries without a direct Rust branch | 213 |

This is a **direct-dispatch inventory of the reference build**, not 213 absent
features or a claim about every release. Many high entries share handlers with
extcall categories that Rust already implements. Their presence is also not
proof that Koikake's current script emits these direct instruction forms. The
previous target-script walk finding core opcodes and 67,196 extcalls does not
establish whole-VM coverage.

- `runtime.rs::step` handles 1..33 and 252/253; the default branch reports
  `UnsupportedCommand` and blocks. High entries cannot be called mere names in
  a catalog: for example, native slot 35 points to the message handler and slot
  40 to the save handler, while direct use would take this Rust default branch.
- Native slot 252 points to `0x444F40`, a timed wait that consumes duration and
  a skip/cancel control operand. Rust treats it as an unconditional one-frame
  wait with no pop. Native slot 253 points to `0x444DE0`, consumes a duration
  and implements timeout/sentinel behavior; Rust emits a plain click wait
  without consuming that value. The native functions also have continuation
  behavior that cannot be inferred from the current primary-opcode labels.
- The 33 core branches are present; this audit does not certify all overflow,
  malformed-script, memory-alias, and task-suspension boundaries.
- In `dispatch_extcall`, a missing handler with a known signature can consume
  the signature's argument count and return zero. That path only emits a trace;
  it bypasses the later `ExtCallSkipped` warning/event. A successful script run
  or absence of skipped-call warnings is therefore not proof of full coverage.

Evidence: [slot inventory](evidence/primary-opcode-coverage.json),
[native dispatch](evidence/runtime-audit/audit_game_44a2f0.c),
[timed wait](evidence/runtime-audit/audit_game_444f40.c),
[click wait](evidence/runtime-audit/audit_game_444de0.c), and current
[`runtime.rs`](../crates/pal-vm/src/runtime.rs).

## Confirmed PAL implementation gaps

| Area | Native evidence | Current Rust behavior |
|---|---|---|
| FX creation and parameters | Fixture `PalSetFxEffect` at `0x100B3B10`, `PalSpriteSetFxEffect` at `0x100C47C0`, and allocator `0x100B3E30` copy an effect-specific `Src` block and record type, time, and state. | `SpriteFxEffect` stores only effect ID, state, one integer value, and active. It has no native parameter block or time progression. `set_sprite_fx_effect` has no caller in the current runtime. |
| FX stop lifecycle | Fixture `0x100B3F20` sets state 2, updates duration/time, and starts an ending phase. Reference release `PalSetFxEffect` independently confirms this. | Action 2 only replaces `value`. It does not set state 2, interpolate an ending phase, or finish automatically. |
| FX rendering | Fixture render callback `0x100B4270` updates the effect and dispatches to 3D or software drawing. The debug reference binds raster/radial uniforms and techniques. | Neither `FrameScene` nor `SpriteDraw` carries an FX descriptor. No renderer reads `SpriteSystem::fx_effects`. State queries cannot substitute for an image effect. |
| Scene effects | Fixture `PalEffectEx` at `0x100B2480` allows IDs through `0x1D`; reference release `0x100A8140` allows through `0x31` and selects an effect handler. | `effect.rs` accepts through `0x31` globally and reduces most visible behavior to an overlay alpha chosen by `effect_id % 4`. The two native builds require version-aware treatment; blindly replacing one bound would break the other. |
| Sprite rotation/render mode | The setters and corresponding fields exist in the native interface and Rust scene data. | Rust stores rotation/render mode, but software blitting consumes an axis-aligned rectangle and color; wgpu builds axis-aligned quads and uses one fixed alpha blend pipeline. The rotated placement branch does not rotate the sampled image. |
| Action timelines | Recovered action handlers have visual lane operations. | `17:6/7/10/11/14/15/17/20` primarily schedule timers/IDs without the native visual commands. `17:5/8/29` additionally apply position/alpha and must not be counted as empty. |
| Save, text, sound, movie boundaries | Separate native evidence is linked from the current port report. | Full native save-image interoperability, several typography/history details, BGV per-lane enable/ducking, exact restored playback position, and unsupported movie variants remain incomplete. |

Fresh fixture evidence is in
[`audit_fixture_100b3b10.c`](evidence/runtime-audit/audit_fixture_100b3b10.c),
[`audit_fixture_100b3e30.c`](evidence/runtime-audit/audit_fixture_100b3e30.c),
[`audit_fixture_100b3f20.c`](evidence/runtime-audit/audit_fixture_100b3f20.c),
[`audit_fixture_100b4270.c`](evidence/runtime-audit/audit_fixture_100b4270.c), and
[`audit_fixture_100b2480.c`](evidence/runtime-audit/audit_fixture_100b2480.c).
Reference release evidence has the separate `audit_release_` prefix; debug
evidence has `audit_debug_`. Do not transplant their addresses between builds.

Implemented CPU palette/grayscale/negative/blur filters, flash/shake, basic
crossfades, and ordinary textured drawing remain real implementations. They do
not cover the separate native FX pipeline. The deliberate `app_exec` no-op also
remains repository policy, not a request to launch host executables.

## Shader translation and actual use

`FX.dll` contains five RCDATA effect resources. The extra manifest resource is
not a sixth shader. [Resource evidence](evidence/fx-resources.json) records
names, offsets, lengths, hashes, and the original `RASTER_SW` source.

| Native resource | Rust source status | Active PAL effect pipeline |
|---|---|---|
| DRAW_PRIMITIVE_UP2D | A WGSL source exists. Ordinary textured drawing is separately implemented by the active generic shader. | This resource's WGSL file is not loaded. |
| BICUBIC | A hand-written cubic filter exists; coefficient/sample parity with the compiled native effect is not established. | Not loaded; current upscale choice is nearest or linear. |
| RADIAL_BLUR | A 12-sample radial formula exists; the native bytecode's taps, coordinates, and constant mapping have not been proved equivalent. | Not loaded. |
| RASTER | Shares the candidate implementation with RASTER_SW based on names, without a bytecode-equivalence proof. | Not loaded. |
| RASTER_SW | Fragment math follows the embedded source, but the WGSL vertex stage adds distortion from code that is commented out in the original HLSL. `stretch` is not an active source parameter. Calling the whole program a direct translation is incorrect. | Not loaded. |

`renderer/shader.rs` only provides a source lookup. There are no calls to
`shader_source` in the renderer. `wgpu_backend.rs` creates one pipeline from its
inline `SPRITE_SHADER`, whose fragment stage is texture multiplied by color.
Its bind group has only texture and sampler, not the effect uniform binding used
by the five candidate files. The five existing files therefore provide **zero
connected PAL FX programs** in the current wgpu backend. This does not mean
that ordinary GPU drawing is absent.

| Host | Current presentation | PAL effect shader port |
|---|---|---|
| wgpu desktop | Active generic textured-quad pipeline | No connected bicubic/radial/raster effect pipelines |
| Software / FFI hosts | CPU `rasterize_scene_rgba` | No FX state consumption |
| Switch | CPU RGBA frame copied to libnx framebuffer | No deko3d PAL effect programs or dispatch |
| PS Vita | CPU RGBA frame uploaded to one vita2d texture | No GXM PAL effect programs; vita2d's own textured drawing is not a PAL FX translation |

Having a native SDK, a buildable NRO/VPK, or a WGSL file is not proof that a
native effect is translated, bound, selected, or produces the same pixels.

## Completion criteria and next work

1. Preserve separate version profiles and evidence for the current fixture and
   reference build. Map actual native calls before assigning completion counts.
2. Unify primary/extcall handler semantics, including operand consumption and
   continuation behavior; expose genuine unimplemented fallbacks in diagnostics.
3. Recover FX parameter layouts, per-frame lifecycle, effect-ID dispatch, blend
   modes, and compiled shader instructions. Correct the RASTER_SW candidate.
4. Carry effect descriptors through the scene model and implement matching
   software, wgpu, Switch, and Vita drawing paths. Compiling a shader alone is
   insufficient; the native VM call must reach it with the right data.
5. Compare controlled native/ported frames at matching parameter values and
   timestamps, including start, stop, clipping, scaling, and blending. Runtime
   and device tests must supplement static verification.

The existing gameplay and unit tests are useful regression evidence, but do not
close these gaps. No full-route, pixel-equivalence, or device certification is
claimed. No code implementation was changed as part of this audit.
