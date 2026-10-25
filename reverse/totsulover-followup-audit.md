# Newer SoftPAL follow-up: text, history, paths, and startup calls

Audited on 2026-10-23 against copied IDA databases. Original game resources
were read-only. All addresses below belong to the newer totsulover build:

- EXE SHA-256: `4778fe70629a7c0ee6cb3caa30ce572cc45440ac392ea998b406ca45ec304483`.
- PAL SHA-256: `744219b7c2b2ba80c5bb0440ac556201652ffbcc01d2de1211c4b576ad21db7e`.

## Startup extcalls and destination semantics

| Coordinate | Handler | Pops | Confirmed behavior |
|---|---|---:|---|
| 18:90 | `0x434100`, helper `0x433D40` | 8 | Debug text formatting at level 0. All eight words are consumed before checking `PalDebugIs`. |
| 5:8 | `0x462350` | 4 | `set_se_info(slot, prefix_handle, volume, extra)`, with four prefix-registry slots. |

Both return native handler success 1 without writing the VM result destination.
That control return must not be mistaken for a script-visible result.

Debug output supports native `%d`, `%x`, `%f` file names, `%t` text records and
uppercase variants, plus decimal width specifiers. The format and seven
additional values are always popped, including release-mode execution.

`set_se_info` checks the slot as an unsigned word and logs an error for values
above 3. The four records have stride 44 at `ctx + 159668`. A prefix handle of
`0x0FFFFFFF` clears enabled, extra, and volume; other handles enable the record,
store the two values, and resolve/copy its filename prefix. The existing native
playback consumer uses the prefix-associated volume. The additional field's
complete playback semantics are not established by this setter alone.

Evidence: [consumers and setters](evidence/totsulover-followup-consumers.json),
[debug formatter](evidence/totsulover-followup-detail.json).

## Path precedence

`PalSystemPathSet` at `0x100BDC90` resolves relative paths against the executable
root, allocates a list node with marker -2, and inserts it directly after the
head. It does not append.

`PalFileCreateEx` at `0x100A9FA0` enumerates marker -2 nodes from head->next,
starting at index zero, and returns the first successful open. Therefore the
last registered path has highest priority. An `ARCHIVE.DAT` profile ending in
`trial` lets that archive override earlier entries. Absolute drive and UNC paths
bypass this list. A fallback open occurs after list exhaustion.

`PalSystemPathClear` removes only marker -2 nodes, preserving other node types.
The shared list also contains other internal state; a portable implementation
may use separate containers while preserving lookup order and ownership.

Evidence: [list mutation](evidence/totsulover-followup-paths.json),
[file lookup](evidence/totsulover-followup-touch.json).

## Text animation mode

The three globals written by `2:47` are consumed by the renderer. They are not
mere configuration latches:

- Type at `0x6F8FE8`: nonzero uses one reveal threshold for every pixel in a
  glyph rectangle (`0x4096A0`); zero uses increasing thresholds across x within
  the glyph (`0x409A50`). Nonzero therefore reveals whole glyph regions while
  zero performs a horizontal wipe.
- Fade at `0x4EB5B8`: enables the 64-unit alpha ramp in `0x409450`. Original
  alpha is copied aside and active sprite alpha is cleared when the threshold
  map is prepared. Once the progress is beyond threshold + 64, the original
  alpha is restored. In the ramp branch the native expression is
  `original_alpha * (max(progress - 64, 0) - threshold + 64) >> 6`.
  Pixels at or beyond the current progress threshold retain their prior alpha.
- Space skip at `0x6F89E4`: affects the threshold-map construction branch and
  completion extent. It does not simply delete spaces from text.

`0x409D00` selects the map generator. Nonzero type uses constant glyph
thresholds. Otherwise, fade disabled or space skip enabled uses per-glyph x
thresholds. The special combination type=0, fade!=0, space_skip=0 builds a
continuous x map over the text surface and advances by surface width for each
line-height block, so blank horizontal distance participates in progression.

`0x409760` visits stored glyph rectangles, expands them for the active font
effect, starts newly animated glyph thresholds at 64, and advances the origin
by each original rectangle's width. Already-displayed rectangles receive zero
threshold. Font-effect geometry and line-boundary rules are visible in the
preserved body and should be ported together with the threshold generator.

`0x46AB70` returns glyph-based versus surface-based reveal extents according to
the same mode test. `0x46BE30` adds 64 when space skip is enabled. Text append at
`0x46DC50` adjusts its timing ratio specially for type=0, fade!=0,
space_skip=0. A port that stores the three flags but keeps the prior uniform
wipe has not implemented these semantics.

Evidence: [direct consumers and callers](evidence/totsulover-followup-consumers.json),
[threshold kernels and glyph traversal](evidence/totsulover-followup-detail.json).

## History face icons: 14:13

This is a history face-icon subsystem, not select configuration and not the
existing `14:12` voice-replay icon. The native label is supported by adjacent
commands `history_set_face_call`, `history_set_face_sound`, and
`history_set_face_sound_release`.

`14:13` pops `(enabled, x_offset, y_offset)`. It normalizes enabled into both
`ctx+352136` and the capture latch at `ctx+352140`, then stores x/y offsets at
`ctx+89936` and `ctx+89940`. `14:14` pops one bool: false clears only the capture
latch, while true restores it from the configured enable flag. Each newly
inserted history record freezes that latch at record offset 488.

The renderer at `0x456350` creates a face icon only when the feature is enabled,
the saved latch is nonzero, and the voice name begins with `VO` ignoring case.
The filename is `IC` plus two bytes after `VO` when the first is a digit;
otherwise it is `IC99`. This is an engine naming convention, not a title-specific
character table. Coordinates use the stored offsets relative to the body's
current baseline placement, including the native ruby/layout x adjustment.

`14:17` stores the click callback point. `14:18` loads its sound resource;
`14:19` stops/releases that resource. Icons are ordinary PAL button entries:
`0x455F20` installs the callback points and uses the shared button lifecycle.
Top-overflowing history entries remove their icon buttons as well as text.

Evidence: [render/capture consumers](evidence/totsulover-followup-consumers.json),
[button creation](evidence/totsulover-followup-detail.json),
[native command names](evidence/totsulover-followup-history.json).

## Second history icon group: 14:24

`14:24` pops `(enabled, x_offset, y_offset)`, storing the bool at `ctx+352144`
and offsets at `ctx+89968`/`ctx+89972`. Unlike face icons, this group uses an
explicit resource and callbacks supplied by `14:22`, which pops five words:
button group, resource handle, push callback point, pull callback point, and
another resolved callback point. The last point's complete consumer remains
unresolved.

During history rendering this icon is created only if enabled, its configured
resource string is nonempty, and the record's field at offset 484 is not -1.
`14:21` returns that saved field for a requested record. The renderer binds the
configured push/pull points through the same native button helper, placing the
icon using its own offsets. `14:23` clears the configured group's active fields.

The record field is captured from native task memory and associated with this
feature. Calling it a jump target or face identifier without tracing those
script callbacks would overstate the available evidence. Keep the portable
representation generic until that remaining relationship is confirmed.

## Touch API distinctions

`PalIsTouch` at `0x100A4760` calls a dynamically resolved Windows touch query for
the window; it returns zero when the API gate is unavailable. `18:89` additionally
checks the digitizer-ready system flag before calling it.

`PalIsTouchMode` at `0x100A46D0` returns the stored word at PAL context offset
648. `PalTouchMode` at `0x100A4670` consumes the low byte of one x86 stack slot,
calls the native registration helpers depending on its state, and stores the
zero-extended byte at offset 648. Its inferred function-pointer return is an
incidental EAX value; it is not a reviewed source-level return declaration.

`PalTouchEnable` is separate: it stores a byte at context offset 652 and is
called by `18:91`. `18:90` is the eight-argument debug-output command described
above; adjacency does not imply a touch-mode function.

Evidence: [touch APIs](evidence/totsulover-followup-touch.json).
