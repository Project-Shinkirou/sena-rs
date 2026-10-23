# Native action scheduler and portable saves

Audited 2026-10-23. The implementation is in
[`runtime/actions.rs`](../crates/pal-vm/src/runtime/actions.rs), with portable
serialization in its `snapshot.rs` submodule. Native decompilation, callback
tables, instruction listings, and binary hashes are recorded in
[`action-native.json`](evidence/action-native.json). This supersedes the old
timer-only interpretation of category 17. Signature status remains `Partial`:
dispatch coverage does not establish complete rendering or wrapper parity.

## Two executable builds

| Property | Koikake executable | Reference Game.exe |
|---|---|---|
| Category 17 table | `0x478378` | `0x4BAC48` |
| Scheduler | `0x40C6B0` | `0x408DF0` |
| Start bank | `0x40C570` | `0x408C40` |
| Finish bank | `0x40C9C0` | `0x408B00` |
| Commit final wrapper lanes | `0x4353A0` | `0x449310` |
| Banks / lines per bank / sections per line | 8 / 32 / 32 | 16 / 64 / 64 |
| Native stage | 1280 by 720 | 1920 by 1080 |
| Sine position entry | Stores coordinates unchanged | Multiplies coordinates by 1.5, truncating |
| Sound start | Uses the supplied fade milliseconds; returns zero | Uses section duration when fade flag is nonzero; returns one |

Runtime build selection currently follows `CoreAssets::extended_softpal`, an
existing script-feature heuristic. That heuristic is sufficient for the two
inspected fixtures; it is not a universal DLL-version detector. The reference
sound wrapper's resource-bank relationship still needs a runtime trace.

## Dispatch contract

Arguments below are in native pop order. An entry appends to a line of the
active bank; it does not start playing or change the active bank.

| Index | Operation | Arguments |
|---:|---|---|
| 0 / 1 / 2 | Run / synchronous run / loop run | bank, duration or cycle count |
| 3 | Clear and finish | bank |
| 4 | Wait through bank | bank |
| 5 | Position | line, sprite, dx, dy, dz, duration |
| 6 | Move to one of 20 stage columns | line, sprite, column, curve, duration |
| 7 | Random shake | line, sprite, axis, amplitude, duration |
| 8 | Alpha | line, sprite, signed delta, duration |
| 9 | Wait section | line, duration |
| 10 | Rotation | line, sprite, dx, dy, dz, duration |
| 11 | Scale | line, sprite, percent delta, duration |
| 12 / 13 | Push / pop one bank backup | none |
| 14 / 15 | Decelerating / accelerating position | line, sprite, dx, dy, dz, duration |
| 16 | Sound | line, sound slot, loop, fade, duration |
| 17 | Periodic shake | line, sprite, axis, amplitude, period, duration |
| 18 | Source rectangle position | line, sprite, cell x, cell y, duration |
| 19 | Source rectangle | line, sprite, x, y, width, height, duration |
| 20 | Curved scale | line, sprite, curve, percent delta, duration |
| 21 | Curved rotation | line, sprite, curve, dx, dy, dz, duration |
| 22 | Null native entry | none |
| 23 / 24 | Set / get active bank | bank / none |
| 25 / 26 / 27 | Run / synchronous run / loop all | duration or cycle count |
| 28 | Clear all | none |
| 29 | Sine position | line, sprite, dx, dy, dz, duration |
| 30 | Mark for deferred clear | bank |

`-1` resolves to the active bank. Bounds and capacity are checked before the
entry handler consumes the remaining arguments. The previous names and arities
at 6, 7, 12, 13, 17, 20, 21, 24, and 25 were misleading or incorrect.
The regenerated [catalog](EXTCALL_CATALOG.md) describes the effective registry.

## Timing and accumulation

Lines run in parallel; sections on one line run sequentially. Deadlines are
cumulative relative to the bank start, but each visual callback measures time
from its own creation. A line crosses at most one boundary per engine update.
The synchronous bank deadline is checked before line callbacks; the ordinary
bank deadline is checked afterwards. Finishing a bank applies the final values
of its current and unstarted visual sections.

The native finite-loop path resets line cursors before finishing its last cycle.
Consequently, its unstarted final callbacks run once more. Infinite-loop
completion is a one-frame latch, reset at the next update, not a permanent
"has completed once" flag. Both behaviors are retained and tested.

Temporary and completed values occupy separate lanes. Bank completion commits
completed lanes globally into base properties and clears that block. Alpha uses
signed 16-bit addition before clamping. Both native commit helpers omit z while
clearing its completed lane; the port retains this omission rather than making
z permanent. Later setters replace committed base values normally.

Curve 1 is the recovered logarithmic acceleration expression (factor 55),
curve 2 the logarithmic deceleration expression (factor 80), and curve 3 a
quarter-wave sine. Position callbacks use sign-biased integer truncation;
linear rotation uses a floating delta. Periodic shake uses integer degrees and
the native single-precision pi constant `0x40490FD8`. Multiple shake callbacks
replace one separate x/y pair in update order; ordinary movement still adds
independently. Rectangle position selects sprite-sheet cells, not raw pixels.

Push/pop use one backup, not an unbounded stack. Deferred clear flags are
consumed when the ADV click-wait stage completes (`0x42D8A0` to `0x40C390`).
Modal menu suspension does not consume these flags.

## Portable snapshot version 11

The existing version 10 body remains unchanged; an appended action block stores
banks, sections, cursors, deadlines, backup, clear/wait flags, temporary/final
lanes, and the random generator state. Scene references are encoded using
sprite slots, button group/index pairs, and ADV body/name bindings. No live
handle is reused after loading. Timers are rebased to the current PAL clock;
ADV bindings wait until text surfaces have been rebuilt.

The same binding block preserves base floating positions, rotation, scale,
offsets, centers, color, projection, render mode, source names, and sampling flags. This fixes
lost action displacement and rotation on load without baking a partial tween
twice. Older portable versions remain readable. This does not fill the missing
native-engine save-image mirror blocks or establish native save interoperability.

## Validation and remaining boundaries

Sixteen focused action tests cover deferred start, line sequencing, stack
arity, native curve values, clamping, bank completion, finite/infinite loops,
forced target moves, transparent-base drawing, active/final save round trips,
clock rebasing, handle reassignment, deferred text binding, and truncated saves.
The real Koikake FFI harness reaches the title and ADV with audio; it does not
exercise every action section in the game.

Outstanding native comparisons include source-rectangle callbacks relative to
clipping state, wrapper child/attached-sprite propagation, sound-bank behavior
in the reference build, malformed negative durations, and exact random-ring
initialization. X/Y rotation lanes are recorded, but full native 3D rotation
and every render mode are separate renderer work. A passing scheduler test is
not evidence that those rendering paths are complete.
