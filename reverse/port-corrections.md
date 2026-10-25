# Runtime port corrections

2026-10-23 follow-up: [action banks and portable save version 11](action-scheduler.md)
and [native scene transitions](scene-transitions.md) replace the earlier action
timer and modulo-overlay approximations. The earlier findings below remain
historical context; the follow-ups list their remaining boundaries.

Follow-up: [VM dispatch and raster FX implementation](fx-vm-implementation.md)
records the later opcode, renderer, and console changes. Test counts below are
the earlier regression scope.

Updated 2026-10-23. This records the current implementation work and its evidence boundaries. It does not certify full SoftPAL compatibility. The target game directory was read-only; gameplay diagnostics used `target/reverse-audit/isolated-game` with its own save directory.

## Corrections with native evidence

| Area | Previous behavior | Current behavior and evidence |
|---|---|---|
| Sprite filters, `3:12` | Consumed operands without changing pixels; documentation confused it with render mode | Game `0x4275F0` maps through `0x49C320` to `PalSpriteSetOption_0` (`0x102604A0`). Palette, grayscale, negative, cross blur, and Gaussian modes transform actual pixels cumulatively. Mode zero restores the original pixels. Native palette and kernels are in `evidence/sprite-filter-tables.json`. |
| Sprite paint, `3:40` | Cleared save overlay bookkeeping only | Game `0x4259C0` consumes six operands. The clipped half-open rectangle replaces RGBA pixels; four zero rectangle operands mean the whole surface. `0x44E370` parses raw hex including alpha, so six hex digits must not acquire an invented opaque alpha. |
| Font resources, `9:15/16/20` | Loader arguments were dropped or only toggled a latch | `0x4383B0`, `0x438390`, and `0x4382D0` load the requested bitmap DAT, unload the bitmap independently, or replace the extended TGA sheet. Extended-load failure clears the previous sheet as native does. |
| Input | Most key bits differed from PAL; cancellation was ignored; input_clear returned time | The native table at PAL `0x103CB130` determines the shared keyboard/mouse mask. Cancellation reaches buttons, tasks, and VM. Clear returns 1 and preserves physical held state without regenerating edges. See [input-control.md](input-control.md). |
| BGV | Shared ADV voice lanes/group; pause ignored | Eight independent lanes in PAL group 2 now support play/stop/fade/pause, cached volume, mute, and text aliases. See [bgv-playback.md](bgv-playback.md). |
| Voice configuration | `13:4` consumed the wrong number of operands; `13:7` was mislabeled as fade | Four-operand registration and the registry enable flag follow `0x443FB0`, `0x444190`, and consumer `0x4425F0`. Playback uses ordered case-sensitive prefix matching, fallback gain/mute, and the native bypass flag. Existing CSV behavior remains for releases that never configure this registry. |
| Window text/debug helpers | Zero/guessed parameter counts, missing savepoint side effect | `15:2` consumes two values and refreshes the native release-mode checkpoint; `15:4` is an eight-operand debug print even when release output is disabled. See [native-helper-contracts.md](native-helper-contracts.md). |
| Desktop window requests | Script changes queued with no host consumer | The winit host applies fullscreen, position, and client-size changes. Native consumption-time enable gating and separate Game requested-mode/PAL accepted-mode state are preserved; resize observations cannot enqueue feedback. See [window-host-requests.md](window-host-requests.md). |
| Tips | Command handlers had no text producer | `<dNUMBER>...</d>` produces records during glyph reveal, custom glyph/effect colors render, and callbacks poll script-controlled geometry. Native does not supply a guessed hover engine. Absolute timed/auto wait deadlines survive callback suspension. See [tips-layout-port.md](tips-layout-port.md). |
| History | Dynamic string handles could resolve to later reused buffer contents | Name/body/voice strings freeze when the record is inserted. Query, render, and replay use the same frozen record. See [history-query.md](history-query.md). |

The input-mask signature now names its second operand `extended_mask`, not `mouse_mask`; mouse buttons belong in the first mask. The former `voice_play_fade` signature is now `voice_info_enable`. The decompiler and runtime share those corrections.

## Portable snapshots

Version 8 adds frozen history strings; version 9 adds sprite filter options and original-pixel backups; version 10 adds Tips state and BGV lanes. Versions 1 through 9 retain their version-gated readers. Filter reset works after restore. Tips snapshots finish only a cloned reveal state and preserve the live typewriter. BGV restoration runs on the next frame even while ADV is parked, replaces outgoing BGV, and keeps ADV voice separate. Details: [bgv-snapshot.md](bgv-snapshot.md).

Review corrections reject overflowing or inconsistent saved sprite/button dimensions before scene restoration. RGBA surface construction also checks multiplication overflow. Tips callbacks validate their point and script bounds before releasing a wait; abandoning an outer callback discards its old timer, while nested callbacks preserve the outer timer.

## Validation

Focused tests cover changed behavior and backward snapshot readers. No Windows, Android, iOS, or release-build certification is implied by local macOS tests.

| Final integrated test scope | Result |
|---|---:|
| `cargo test -p pal-vm --lib` | 144 passed, 4 ignored |
| VM integration binaries: audio, engine, input, movie timing, MPEG-2, runtime, save semantics, sprite, system, task | 85 passed |
| `cargo test -p pal-script` | 14 passed |
| `cargo test -p pal-decompiler` | 33 passed |
| `cargo build -p pal-vm --bin sena` | passed |
| `git diff --check` | passed |

These scopes total **276 passed and 4 ignored**, without double-counting focused reruns. Logs are in `target/reverse-audit/port-final-{vm,script,build}.log`.

The real-game smoke run used a fixed 16 ms timestep, 1,600 frames, GAME START at logical `(160,260)`, and automatic ADV input. It reached the bedroom scene and advanced to `0x6B82C`; the final image was inspected. It is a startup/early-play check, not a complete route playthrough or audible audio test.

A subsequent 2,100-frame run clicked Q.SAVE at frame 1,700. The isolated root produced `save/save999.dat` with a version-10 `SENARSAV` trailer and resume PC `0x6B858`, then continued to `0x6BB34` without a runtime failure.

A fresh process then clicked Q.LOAD when its button became enabled. At frame 1,236 it restored slot 999 at `0x6B858`, rebuilt the scene and parked line, replayed the saved BGM on the next frame, and advanced into the next scene. The final image was inspected. Logs: `target/reverse-audit/port-save-gameplay.log` and `port-load-verified.log`. A prior fixed-frame load click landed during a transition and did not activate a button; it is not counted as a restore test.

## Remaining work

- [The follow-up implementation](fx-vm-implementation.md) closes the audited primary dispatch routes, 252/253 operand handling, and type-0 raster lifecycle/rendering. Complete scene-effect IDs, radial/bicubic dispatch, render modes, action animations, and Vita GXM shaders remain incomplete. The fixture and reverse reference are different builds; native addresses and effect-ID bounds must stay scoped by build.
- Complete native save-image import/export: sprite/history/text-window/sound wrappers and mirrored blocks are not fully mapped; native engine acceptance of written saves is unverified.
- MP4 playback is not established in the verified native profiles. The former native-support claim conflated a separate debug DLL with release DLLs; its H.264 graph is incomplete. Game's automatic suffix lookup is WMV/MPG/AVI; MP4 was removed from that lookup. See [movie-format-policy.md](movie-format-policy.md). WMV/MPEG support still has explicit unsupported codec variants.
- Several action timeline and text effect handlers still model timers/state without the recovered visual behavior. History voice icons, kinsoku, and overflow ellipsis remain absent.
- BGV per-lane enable/auto-ducking, exact restored playback position, and instantaneous fade gain remain incomplete.
- Sprite filter replacement-image replay and SIMD rounding parity remain unverified. Small-image blur bounds are safely clipped rather than reproducing native out-of-bounds access.
- Dynamic font resource/type/effect changes are not serialized; restoring after a different font was loaded can retain outgoing metrics. Extended sheet support is used by wait marks, not a complete generic extended-glyph implementation.
- Select cursor warping and suspended/intermediate choice restoration remain partial. Fatal-frame message boxes are wired, but native error sites are not all mapped.
- Input async-key queries use events delivered by the host rather than polling global OS keyboard state.
- Desktop native aspect modes and external FFI host window-request consumption remain incomplete. OS acceptance of the new desktop operations has not been exercised with GUI automation.

Use [EXTCALL_CATALOG.md](EXTCALL_CATALOG.md) for a deduplicated signature inventory. Its status counts cannot be converted into a count of missing runtime handlers. The earlier [runtime-gap-audit.md](runtime-gap-audit.md) is explicitly a historical starting snapshot.
