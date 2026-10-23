# `history_get_text` (category 14, index 20)

Implemented from `reverse/Game.sqlite`, not from the misleading previous stub.

## Verified native contract

`sub_42C460` pops four arguments in this order: record index, destination name string handle, destination voice string handle, destination body string handle. All three destination operands must have dynamic-string bit `0x10000000`; validation happens before any string copy. It writes the three strings and returns the saved text instruction PC, **not the history count**. Invalid destination type gives zero.

Native record stride is 4428 (`0x114C`) bytes, starting at `ctx+88452`:

| Relative offset | Meaning | Producer evidence |
| --- | --- | --- |
| 0 | Speaker name | `sub_43D9D0` / `AdvTextSetName` |
| 255 | Body text, including text markup | `sub_43D770` / `AdvTextSetText`; `sub_43D350` preserves markup |
| 4351 | Voice resource name | `sub_43C2C0` / `text : voice` |
| 4416 | Saved instruction PC | `sub_43F900`, `sub_43FC60`, `sub_43FFC0`, `sub_440390` |

Those text handlers copy `PalTaskGetTaskData(0)+805048`. The VM context starts at task+824, hence this is context+804224, named `saved_pc_trace` in `STRUCTS_RE.md`. `sub_44A2F0` assigns that field from context+804060 before fetching each opcode. It is the byte offset of the text instruction in the script, not a POINT.DAT index or host pointer. Native insertion shifts older records upward; index zero is newest.

## Rust implementation

`runtime/history_query.rs` reads name/body through the script text resolver, voice through the resource resolver, validates destinations atomically, and converts native newest-first indexing to Rust's oldest-first vector. The previously unused fifth history dword (`record[4]`) now captures `pc-12` at text submission. All text extcalls are 12 bytes; existing snapshot serialization already persists all nine history dwords, so the PC field itself needed no format revision. Earlier snapshots contain zero in that field and therefore return zero for their unavailable original PC.

Out-of-range indices return zero without writes. Native does not validate these indices and may read outside its history array; that unsafe behavior is intentionally not reproduced.

## Frozen source strings and save version 8

New history entries now copy resolved name, body, and voice into `FrozenHistoryText` at submission. Rendering, `history_get_text`, and click-to-replay all consume those copies. Reusing a dynamic string buffer no longer edits an earlier line or redirects its voice. The old handle-based deduplication was removed: two separate submissions with the same reused handles are distinct records. The frozen array follows the 128-record eviction order and is cleared by `history_clear` and `history_init`. Importing an original-engine save clears the outgoing backlog because that import does not map native history yet.

Version 8 appends a record count and, per history record, a presence byte followed by length-prefixed UTF-8 name, voice, and body strings. The count must match the existing record table; strings are bounded at 16 KiB each and strict UTF-8. Versions 1–7 remain readable. Their static handles still resolve against the game assets; dynamic contents were never serialized and are returned empty rather than accidentally resolving an unrelated buffer from the outgoing scene.

**Language behavior:** newly recorded history preserves the selected language at insertion, as native copies the displayed string. A later language switch still changes fonts/layout but does not translate existing version-8 history. Legacy records without copies retain static-resource resolution in the current language. This supersedes the earlier description that all backlog text is reselected on a language switch.

## Verification and remaining limits

`cargo test -p pal-vm --lib runtime::history_query`: 9 focused tests cover source-buffer reuse, identical-handle submissions, frozen rendering and voice lookup, save/restore into a fresh VM without dynamic buffers, 128-record eviction, clear, a real version-7 payload without the new suffix, malformed UTF-8/count validation, and the previous query/PC tests. A synthetic Sv20 program exercises the actual push/text/query instruction path across the ADV wait boundary.

Original-engine history import, name substitutions, and numeric substitutions remain incomplete. `sub_43D350` contains the numeric memory-bank lookup and full-width digit table, but this build's decompilation mixes UTF-16 checks with narrow-byte parsing; no new guessed parser was introduced. Voice icon sprite creation from `history_set`, kinsoku, and overflow truncation also remain separate work.
