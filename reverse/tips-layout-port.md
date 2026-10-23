# Tips layout and callbacks

The command state in `runtime/tips.rs` now receives records from the ADV layout
pass. A submitted line schedules one capture; alpha changes, reveal completion,
wait-mark animation and modal return do not duplicate its records. `<d42>term</d>`
and `<d>term</d>` produce an ID (42 or -1), the raw enclosed string, and two raw
substrings when the term wraps. Opening slot 31 wraps to slot zero, as in
Game.exe `0x43C7C0`. The two-string split retains the final wrap, matching the
native overwrite on each wrap. Tags are removed from displayed body text and
reveal-length calculations.

The eleven dwords have specific meanings: ID, start x/y, first-rectangle end
x/y, wrapped-rectangle start x/y, wrapped-rectangle end x/y, font-cell width,
and line pitch. The final two are not the term's overall width and height.
The existing query code adds the native font-effect padding separately.
Geometry shares the portable ADV renderer's character boundaries, so query
rectangles follow what this renderer actually displays. This does not establish
pixel parity with the native fixed-cell layout.

`0x439480` does not perform mouse hit testing. It calls the registered script
point while the low mode byte is set and sets the Tips flag to 1. Scripts can
query the records and poll mouse position themselves. `0x43DB40` invokes the
second callback on ADV completion when the high mode byte is set, then sets
flag to zero. The portable engine now runs the polling callback through its
modal continuation mechanism and re-parks the same ADV wait on return. It
prevents recursion while a callback is active. Completion is queued once when
the outer ADV click wait resolves; an inner callback/menu wait cannot complete
that suspended line.

Records are staged against the same character offsets as the smooth reveal.
An opening tag exposes the pending slot's ID/start position; completion count
and strings change when its closing tag's glyph boundary is reached,
so the polling callback can start before the rest of the line is visible.
Skipping/reveal completion activates the remaining records. Automatic and timed
waits preserve their original task clock across callbacks, including wrapping
32-bit timestamps and the auto-mode voice gate. Expired or pending-free tasks
cannot be repeatedly replaced by polls. Resuming a Tips wait does not add the
remaining text reveal duration a second time.

Configured Tips text/effect colors now recolor the tagged glyph cells in the
cached text block. The normal font colors are restored afterward, untagged cells
remain unchanged, and disabling custom colors invalidates the cache to restore
the normal appearance.

## Remaining differences

- A click that advances the line delivers completion without a final poll;
  native task order can run both callbacks in that update.
- Native kinsoku, inline size/ruby geometry, clipping/overflow behavior and
  font-cell pitch do not yet share an exact native layout implementation.
- Colored glyph cells follow the portable renderer's boundaries; native
  effect overhang and inline font changes still need pixel comparison.
- Strings use safe Rust allocation rather than reproducing unchecked native
  writes into 2047-byte records. Malformed unclosed terms do not create records.

Focused tests cover record lanes, raw strings and wrap split, wrap-boundary
endpoints, ring rollover, redraw deduplication, callback re-parking and completion,
unknown tags, per-term slow reveal, colored glyph pixels, and finite auto
completion across repeated callbacks and clock wrap. Run
`cargo test -p pal-vm tips::tests --lib`.

## Portable save version 10

The version-10 tail stores Tips immediately after the version-9 sprite filter
section and before BGV. Its marker is `TIP1`, followed by the completed-record
cursor, two callback point IDs, mode, pause/color/completion flags, two colors,
and exactly 32 records. Every record contains eleven signed dwords, a strict
boolean wrap flag, and three length-prefixed UTF-8 strings. The reader validates
the cursor (0 through 31), the exact record count, 16-bit mode, boolean flags,
UTF-8 and an 8192-byte limit per string before accepting the state. The writer
rejects oversized strings instead of silently truncating them.

Save capture clones Tips and finishes staged opening/closing events only in the
clone, matching the fully revealed line restored from portable saves. The live
reveal and its query count continue unchanged. A resumable checkpoint can be
created before the first draw; that path resolves the body already available to
the checkpoint and produces its layout with a cloned font before completing the
snapshot. This prevents the first-frame checkpoint from dropping every Tips
record on that line.

Restoration preserves callbacks, flags, configured colors and every record,
including unused slots retained by native reset semantics. It clears deferred
record production and all host timer state, so the forced ADV redraw cannot
append duplicate records. The completion latch remains armed for the restored
ADV wait. Old versions 1 through 9 load an empty Tips state, and original-engine
save import also clears it rather than inheriting menu/title records.

The tests include a full portable snapshot encode/decode/restore roundtrip during
slow reveal, pre-render checkpoint capture, restored string search/extraction,
completion callback and reset/color behavior, a version-9 file physically
truncated before the new tail, and malformed counts, flags, UTF-8, lengths and
truncated payloads.

## Callback lifecycle review

Abandoning the outer modal continuation now discards both suspended and resumed
Tips clocks. An unrelated later menu cannot inherit that expired deadline.
Abandoning only an inner modal preserves the still-active outer Tips clock.
Polling validates its registered point against the current point table and
script bounds before releasing the ADV wait; invalid IDs, arithmetic overflow
and out-of-script destinations leave the existing wait parked.

Regression tests cover both abandonment cases and invalid callback destinations.
