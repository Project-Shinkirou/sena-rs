# Input, buttons, cursor and touch contracts

The reviewed inventory is [contracts/input_buttons.json](contracts/input_buttons.json):
50 exact exported names, 132 export/build instances, eight newer VM wrappers,
and the separately inspected Koikake input-clear wrapper. Every export instance links an instruction body with the same
binary hash and address. `behavior_status: reviewed` means the documented export
boundary was inspected; it does not certify the portable implementation or all
downstream operating-system behavior. Several setter return types remain partial
because incidental EAX values do not establish an original C declaration.

Builds are Totsulover PAL SHA-256
`744219b7c2b2ba80c5bb0440ac556201652ffbcc01d2de1211c4b576ad21db7e`,
reference PAL `e676972ddaab2d45fc82210bb89143923f10d1053492a89e50571dbcec54840a`,
and Koikake PAL `2a43bc4408aff086c18dc73ccd2d35be4f8a56cf8b3e82da7b75f80ffe61625e`.
Original game files were not modified.

## Input sampling and consumption

The sampling equations are `push = current & (current ^ previous)`,
`on = current & previous`, and `pull = previous & (current ^ previous)`.
`PalInputGetKeyOn` therefore excludes the first sampled frame of a press. The
portable `raw_key_on` already has a separate stable mask; the more convenient
`key_on`/`mouse_on` methods include the immediate press and must not silently
substitute for this native export when reproducing its consumers.

`PalInputGetKeyOnEx` is a repeat **level**, not an operating-system repeat event.
Every new push replaces the remembered push mask and timestamp. If no new push
occurs, any intersection of the remembered mask with stable held bits keeps the
repeat candidate alive. After strictly more than 500 ms it returns the entire
remembered mask. It does not reduce that mask to the surviving held bits and it
does not impose a 60 ms interval. `PalInputKeyRepeatClear` zeros both repeat
output and remembered mask. `PalInputClear` clears current exported key and
joystick masks but preserves physical sample history, repeat history, wheel,
extended input, coordinates and cancellation masks.

Evidence: new sampling `0x100ACEC0`/`0x100AD010` in
[input-totsulover-processing.json](evidence/input-totsulover-processing.json),
reference `0x100AB690` in
[input-reference-processing.json](evidence/input-reference-processing.json),
Koikake `0x100B7C80`/`0x100B7DF0` in
[input-koikake-processing.json](evidence/input-koikake-processing.json).

`PalInputKeyCancel(key_mask, extended_mask)` stores persistent masks and also
immediately removes the requested current outputs. The first mask includes mouse
button bits 23..25. The second masks extended events; bit `0x10` clears wheel.
Setting a cancellation mask back to zero cannot resurrect a cleared edge in the
same sample. Raw setters OR into only their named output word and do not alter
physical key sampling history.

`PalInputGetKeyEx` reads the extended event word: double-click left/right/middle
are bits 1/2/4 and window-mode change is bit 8. Wheel is a separate signed 16-bit
latch. All three window procedures map a nonnegative Windows wheel delta to PAL
`-1` and a negative delta to PAL `+1`; zero follows the nonnegative branch. The
newer engine message consumer overwrites the wheel latch with the last queued
event and clears it after the frame callback. The getter returns AX without
defining upper EAX bits, and consumers explicitly test/sign-extend the low word.
Thus ordinary wheel down advances the story, because the script scheduler tests
native wheel `>0`. Forwarding winit's positive-up delta directly reverses this.
See [reference window](evidence/input-reference-window.json),
[Koikake window](evidence/input-koikake-window.json), and newer window/thread
functions `0x100A0910`/`0x100A2780` in the processing evidence.

Native coordinates use single-precision scale calculations truncated toward
zero. The sampling routine rejects coordinates `>= logical_extent` or
`< -logical_extent`, replacing that axis with `-1`; other negative coordinates
can survive, though button GetReaction explicitly rejects negative x/y. Delta
is zero when the resulting coordinate is `-1`, otherwise current minus the
previous stored coordinate. Inactive/minimized/resizing state zeros input and
deltas and sets position `(-1,-1)`. The portable coordinate conversion now uses
float32 truncation and the native signed bounds. Host focus/minimize/resize
sampling still needs the same gate; conversion tests do not certify host delivery.

Joystick getters read four stored masks only when enabled. DirectInput polling
maps X/Y strictly below -800 or above 800 to direction bits 1/2/4/8 and places ten
button bits above those. The inspected poller does not populate JoyOnEx; do not
invent keyboard-style repeating for it. No joystick device certification follows
from these export contracts.

## System-button wrapper differences

The newer system-button poll at `0x467390` allows held arrow bits `0x780` while
rejecting other stable held keys. Koikake `0x4291F0` and reference `0x438EA0`
reject **any** nonzero stable KeyOn before polling shortcuts. All inspect
`push | (entry.repeat_mask & KeyOnEx)`; the entry word is a mask, not a boolean.
The newer/reference release-mode mouse shortcuts use pull instead of push.
The first eligible matching slot dispatches, and a shared 50 ms gate is set
when the scheduler reports no skip/input state. These are not independent
per-slot 400/60 ms repeat timers. The old stable-key gate also means that the
presence of a repeat-mask calculation alone does not prove held-key repeat
delivery in those EXEs.

Evidence: [newer wrappers](evidence/input-totsulover-exe-wrappers.json),
[reference wrappers](evidence/input-reference-exe-wrappers.json),
[Koikake wrappers](evidence/input-koikake-exe-wrappers.json). Native PAL key
repeat and Game.exe shortcut dispatch are different interface layers.

## Native button state and ownership

Button registration borrows sprite pointers. Group release destroys registration
nodes and its six owned sound handles, but never releases registered sprites.
`PalButtonCreateEx` stores and later releases both sound handles supplied by the
caller, as well as four copies of the hover sound; successful creation therefore
transfers ownership of those original handles. The filename constructor contains
an actual copy-source error in all three releases: it invokes SoundCopy on each
zeroed destination slot, so the additional hover voices normally remain empty.
Do not describe the intended five-voice pool as the executed filename behavior.

Registration nodes are inserted at the list head. Hit selection tests every
node, including disabled nodes, and chooses strictly the smallest native sprite
priority. The first encountered equal-priority hit wins. If that front node is
disabled, the answer becomes null and the node occludes enabled nodes behind it.
Current portable `button_hit_at` skips disabled entries and therefore allows
click-through. A portable renderer may invert priority values, but the ordering
and disabled occlusion must still match its adapter.

The shared reaction output is exactly 12 bytes:
`{completed_sprite, hovered_sprite, pending_sprite}`, three borrowed 32-bit
native pointers. It is not a list of arena IDs or indices. `GetReaction` returns
the completed sprite pointer. Starting a click sets pending and starts its
sound; a later poll waits until the click sound is not playing before moving
pending to completed. Even absent sound does not complete in the same poll.
`SetReaction` creates the same pending state with a flag to start sound on the
next poll. `IsProcessing` tests pending. Native exports invoke no VM callbacks.
The EXE's button poller clears input while pending, dispatches a click callback
after completion, and separately tracks hover entry/exit callbacks.

Newer push/repeat flags are bit 1 / bit 2, bit 4 suppresses automatic image-cell
changes, bit 8 suppresses sound in ordinary GetReaction, and `0x10000` is passed
to SpriteHit. Repeat is considered only when push-bit 1 is absent. Reference and
Koikake do not implement the newer bit-8 sound suppression. Newer release mode
ORs flags with 2; reference release mode assigns flags=2, discarding the other
bits. Koikake has no exported global release-mode control.

GetReactionEx uses explicit coordinates and trigger, ignores global mode and
the push/repeat flags, and does not honor sound-suppression bit 8. Its null-group
branch unconditionally zeroes the caller output; unlike ordinary GetReaction,
null group plus null output is not accepted. SetPos/GetPos concern the base
image-cell row, not screen position. Reset clears pending/current/previous
reactions and capture; an optional matching sprite is disabled. Reset-only still
returns zero. Delete removes only the first matching node and does not reset a
pending reaction.

Evidence: [newer exports](evidence/pal-core-totsulover.json),
[newer helpers](evidence/input-totsulover-helpers.json),
[reference exports](evidence/input-reference-exports.json),
[reference helpers](evidence/input-reference-helpers.json),
[Koikake exports](evidence/input-koikake-exports.json),
[Koikake helpers](evidence/input-koikake-helpers.json).

## VM coordinates must remain distinct

Newer category 8 index 12 at `0x4237E0` is the slider-end latch, despite older
catalog text calling it `btn_on_check`. It returns -1 on left-button pull or an
already-ended group, otherwise 0. Category 8 index 38 at `0x425F20` has the
misleading native debug text `btn_on_check` but queries the group's completed
reaction and returns boolean 0/1. The latter checks that the requested slot
exists but does not compare the returned sprite to that slot. Do not change
index 12 to boolean semantics based on the duplicate debug label.

Newer 8:17/23 read stored clicked/hovered index latches, 8:36 changes button mode
without writing the destination, 8:39 reads group toggle after a slot existence
check, and 8:40 writes six information words to the task bank. Newer 9:35 clears
input and repeat history but also has no destination store. Koikake wrapper
`0x428090` does the same: its return 1 is VM control flow, not a destination
value. The body and its data xref `0x478B64` are recorded in
[input-koikake-vm-extra.json](evidence/input-koikake-vm-extra.json). The portable
adapter now preserves the destination and clears both histories. Registration
and wrapper bodies are linked separately in the JSON inventory.

## Cursor, touch and unresolved gesture fields

Cursor SetPos performs an OS warp and temporarily stores the physical screen
coordinates until the next input sample. AutoHideTime sets/restarts timer
10000; it does not immediately change visibility. SetCursor mode 1 loads a
system cursor; other modes load a TGA resource and use the supplied hotspots.
Native success return 1 does not prove OS cursor allocation succeeded.
ReleaseCursor destroys the stored cursor but does not clear the window-class
cursor; the decorated `?PalDeleteCursor@@YAXXZ` clears the class cursor but does
not destroy the stored object. Their ownership effects are different.

IsTouch queries window registration through Windows. IsTouchMode reads a stored
mode; TouchEnable writes an independent byte gating touch messages. TouchMode
has an unusual verified branch: new zero mode only calls RegisterTouchWindow
when the old stored mode is already zero. Switching from nonzero to zero does
not re-register there. OS result is ignored. This must not be silently rewritten
as a conventional enable/disable setter while claiming native equivalence.

GetGesture copies an opaque 48-byte buffer, and SetGesture writes only its first
word. GestureCheck inspects five different 48-byte records and tests whether
their first word is in 3..7. The record producer and remaining fields are not
established; assigning Windows GESTUREINFO names to the bytes would be a guess.

PalSetPow belongs to rendering: the graphics audit confirms that its byte forces
power-of-two GPU dimensions independently of device capabilities, with square
render targets. Its original C return declaration remains unresolved because
only AL is defined. Consumer evidence lives in
[newer platform consumers](evidence/pal-platform/totsulover-consumers.json) and
[reference platform consumers](evidence/pal-platform/reference-consumers.json).

## Portable corrections and validation

The portable input path now latches the last wheel direction, reproduces the
500 ms repeat level and whole-mask retention, distinguishes Clear from
RepeatClear, and preserves physical state across cancellation. Native skip
sampling excludes a fresh Ctrl press and suppresses click pushes while Alt is
stably held. The engine samples repeat once before scripts/buttons/tasks.

System shortcuts now select the first matching slot in native order, interpret
the entry repeat word as a bit mask, apply the older/newer stable-held gates,
and share a 50 ms idle gate. Native slot 1 is negative wheel (physical up); slot
2 is positive wheel (physical down). Reference/newer release-mode mouse slots
consume pull instead of push. A disabled front node occludes buttons behind it;
equal-priority hits use registration order, retained by portable save version12.

The delayed reaction path now owns and releases group sounds, copies five hover
buffers, selects the first idle buffer, starts synthetic clicks on the next
poll, and waits for click audio completion before dispatching. Button flags
retain their native words: push/repeat precedence, automatic-cell suppression,
newer sound suppression and optional per-texel hit testing. Disabled controls
retain their current alpha/cell. Hits compare stored base priority, with the
Game adapter's explicit native-depth sign inversion, and ignore modulation
alpha through `SpriteSystem::hit`.

Koikake's complete button table at `0x478590` is recorded in
[button-koikake-vm-table.json](evidence/button-koikake-vm-table.json), including
all 26 nonnull entries through index26. The following table is excluded. The
mutating wrappers return VM control1 without storing to the ordinary destination;
only indexes9/12/17/23/25 call destination helper `0x4377D0`. Index14 writes
explicit temporary-bank words instead. Hover entry/leave callbacks use return
mode0; click callbacks use mode1. A click's leave handler executes before its
click continuation, with no interrupted-script instruction between them.
[Caller/return evidence](evidence/button-koikake-hover-context.json) includes
raw disassembly because the decompiler loses one `PalListPush` argument.

Portable save version12 retains registration order, raw button flags, base row,
hover callback coordinates, and group latches/lock clocks. Newer child-target
state is appended separately in version13. Version14 also retains the raw group
toggle and independent wrapper base/slider offset words. No additional regression tests were
added after the user's instruction; subsequent verification is compilation and
isolated actual gameplay. Host device/focus/cursor/gesture delivery and exact
per-group concurrent polling remain separate integration boundaries.

## Verified wrapper corrections

Newer toggle setter `0x423630` stores its unmultiplied argument in the group
word queried by `0x426080`; the render command separately sets the button base
row to three times that argument. `0x426180` writes wrapper base X/Y, cell
width/height, and texture width/height. The last pair is not a source-rectangle
origin. Missing entries preserve the destination; bound errors follow their
individual native control/output paths.

Koikake slider setter `0x40EC60` multiplies signed32 words with wrapping then
divides by100. It does not clamp. The verified path stores separate base and
slider positions and does not infer an anchor from another button. The query
`0x40ED20` returns the clamped raw offset before storing its peculiar vertical
snap (adds a nonzero remainder); its horizontal branch adds zero. A mouse
coordinate of-1 returns the previously stored axis offset. Unknown profiles
retain the older compatibility path.

`8:38` consumes a PAL reaction without scheduling the Game click callback. It
checks the requested slot exists but queries the whole group. Native control0
now ends the current VM slice, without reporting an unsupported call. Slider
wait-control likewise yields after writing0 and continues after writing-1.

Release of an active hover invokes its leave callback before deleting the entry.
Pending sound/reaction state is per group; other groups can update hover while
a click sound waits. Host focus transitions and exact native callback return-mode
flags still need native scheduling comparison.
