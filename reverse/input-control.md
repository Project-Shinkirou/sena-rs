# Native input masks, cancellation, clearing, and queries

## Evidence

Game.exe `sub_437E70` (category 9 index 18, `key_canncel`) pops two masks in order and stores them at context dwords 201059/201060 before calling `PalInputKeyCancel`. The second argument is an **extended-input mask**, not a mouse-button mask. `PalInputKeyCancel_0` (`PAL.sqlite`, 0x10247450) clears selected bits from key push/on/pull/repeat and key-ex state; extended bit `0x10` clears the wheel. `sub_10246500` reapplies the stored masks every input sample. `sub_10273B10` is exactly `value & ~mask`.

The exact key-bit order comes from the DWORD virtual-key table at `PAL_d.dll` address `0x103CB130` (the database references the debug image; the release `PAL.dll` has different offsets):

| Bits | Inputs |
| --- | --- |
| 0, 1, 2, 3 | Escape, Space, Return, Ctrl |
| 4, 5, 6 | Shift, Alt, Tab |
| 7–10 | Up, Down, Left, Right |
| 11–22 | F1–F12 |
| 23–25 | Left, right, middle mouse |
| 26–30 | Y, N, Z, X, C |

The alternative table at `0x103CB1B0` swaps left/right mouse for the Windows swapped-buttons preference. Portable event names use the host's semantic button identity.

`sub_10246950` computes push as current-and-not-previous, pull as previous-and-not-current, and on as current-and-previous. New presses therefore appear in push, not on, until the next sample. The public script mask getters implement that distinction; high-level portable `key_on` remains the existing physically-held convenience accessor.

Game `sub_437890` (9:35) calls `PalInputClear` and `PalInputKeyRepeatClear`, clears its text-task forced-redraw latch, and returns success. It does not return the clock. PAL's clear zeroes exposed key state but preserves its physical current/previous samples, mouse position/deltas, and wheel. A held key must not become a fresh press after clear.

| Command | Native function | Pop count | Result |
| --- | --- | --- | --- |
| 9:32 `get_pull_key` | 0x4379E0 | 0 | Combined key/mouse release mask |
| 9:33 `get_on_key` | 0x4379A0 | 0 | Combined stable-held mask |
| 9:34 `get_push_key` | 0x437960 | 0 | Combined press mask |
| 9:35 `input_clear` | 0x437890 | 0 | 1 |
| 9:54 `get_async_key` | 0x4378D0 | 1 | Physical Win32 virtual-key state, 0/1 |

## Implementation

`input.rs` separates physical held state from the sampled public states. Mask changes clear current sampled bits immediately and persist across frames without losing physical state. Removing a mask in the same frame cannot resurrect an already cancelled edge. Extended bit `0x10` filters wheel events. Async polling bypasses PAL cancellation.

`runtime/input_control.rs` stores runtime-lifetime masks and frame-local suppression. Every VM instruction receives a filtered input snapshot, so a cancellation/clear followed by a query in the same script slice sees the mutation. The engine applies controls before button/task processing and transfers script mutations back to the host input immediately after the VM slice. Existing consumed-click snapshots continue to work.

The previous guessed keyboard bit assignments were replaced with the native table. Mouse buttons are included in script getters at bits 23–25 while the high-level mouse accessors retain their existing three-bit API.

## Verification and boundaries

Tests execute a real Sv20 sequence containing query, cancel, async query, clear, unmask, and another query. They also exercise exact key bits, physical repeat after clear, stable-held versus pushed state, persistent filtering across frames, button/ADV consumer predicates, and wheel cancellation.

The host event abstraction currently lacks focus-change events; async polling can report delivered physical events but cannot independently query another application's keyboard state or emulate native `IsActive`. Unmodeled virtual keys return false. PAL key-ex window/menu events, joystick input, and native key-repeat timing are not newly implemented. The native text-task forced-redraw latch at context+4192 has no separate portable field; input clear does not erase the renderer's unrelated dirty-cache flag. Cancellation masks are runtime state and do not add a portable-save field/version.
