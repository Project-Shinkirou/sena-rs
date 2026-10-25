# Koikake native save import

The complete stream reader is selected only when the PAL and Game binaries
both match the audited Koikake SHA-256 pair. Unknown binaries retain the older
header-only compatibility fallback. Names and export capabilities do not select
this serialized layout. See `assets/native_profile.rs` for all three exact pairs.

The writer is Game.exe `0x424EA0`; the corresponding reader is `0x424530`.
The original binary SHA-256 is
`93f228aa413aece0f047b27ddb1ea26d4e50316304e6c4029eae4a1a844f437b`.
The saved stream is a sequence of fixed and conditional records, not a memory
dump with one universal offset table. Thumbnail length changes every following
offset. Evidence is in `evidence/koikake-save-mirrors.json` and
`evidence/koikake-save-subsystems.json`, including assembly for each reader and
writer. Supporting operand, text, and lifecycle traces are in the other
`koikake-save-*.json` files.

The earlier assertion that native saves contain no memory or return stack was
incorrect. `0x424530` restores the user bank and operand/argument storage, then
rebuilds a list of return addresses. `TEXT_VALUE_OFFSET` at `0x12A3C` happens to
read the fourth operand-buffer word for a 128x72 thumbnail. It is not a dedicated
current-text field. Native `0x42D170` reconstructs the visible text from the
newest saved history entry through `0x41FF40`.

| Stream section, after the thumbnail | Native reader | Portable handling |
| --- | --- | --- |
| 0x1000-byte user bank | `0x424530`, operand resolver `0x421000` | Restore 1024 signed words; preserve the separate system and temporary banks. |
| 0x8004-byte private bank | `0x424530` | Consume and report nonzero data as an unresolved restoration field. |
| Dynamic-string cursor and eight 255-byte strings | `0x424530`, allocator `0x4143C0` | Strict NLS decode, restore ring contents and cursor. |
| Two dispatch words, operand count, 64 operand words | `0x424530` | Restore bounded operand storage; dispatch words consumed. |
| Argument base, count, 512 argument words | `0x424530` | Restore bounded argument storage and full signed base. |
| Fixed 40-byte subsystem block | `0x4166C0` | Consume; no unsupported field names inferred. |
| Four optional button groups, up to 128 entries each | `0x40FD00` | Reopen images, retain click/enter/leave callback points and wrapper base/slider positions. PAL registration defaults to flags 1. |
| Select/process/effect blocks | `0x427820`, `0x421E00`, `0x412110`, `0x418800` | Consume; active effects produce an explicit limitation diagnostic. |
| 128-byte sprite bank state, 64 optional sprite records | `0x41E210` | Reopen static images, including GRAPHIC.DAT replacement names; restore available snapshot transform/color/visibility fields. |
| Two 92-byte BGM records | `0x40DBE0` | Reopen playing BGM resources with looping enabled, as the native loader does. Nondefault gain, paused state, and byte ranges are reported as missing restoration state. |
| SE shared fields and sixteen 64-byte records | `0x4263E0` | Consume; resumable SE resources are explicitly reported as missing. |
| BGV shared fields and eight 76-byte records | `0x431560` | Reopen active BGV lanes using the native looping mode. |
| History count and 128 records of 542 bytes | `0x420050` | Restore name/body/voice strings as frozen backlog text; reverse native newest-first order to portable append order. |
| Conditional ADV window/configuration and icon records | `0x42D170` | Restore text geometry, base resource, alpha, and current line. Icon animation remains reported as missing. |
| Child-process records | `0x40C5E0` | Consume bounded records; active processes are reported as missing. |
| Return count and addresses | `0x424530` | Restore return-address order, reject addresses outside the loaded script. |

The 76-byte `PalSpriteInfoKoikake32` declaration is in `abi/save32.json`, before
the importer implementation. The Rust record has `repr(C)` and compile-time
size/alignment/offset assertions. Serialized words are read explicitly as
little-endian data; native pointers and old PAL handles are never dereferenced.
Native `PalSpriteGetInfo` / `PalSpriteSetInfo` addresses are `0x100C29D0` /
`0x100C2B00`, respectively, in PAL hash
`2a43bc4408aff086c18dc73ccd2d35be4f8a56cf8b3e82da7b75f80ffe61625e`.

`runtime/native_save.rs` caps native images at 2 MiB, bounds every conditional
record and count, rejects malformed floating-point transforms, requires the
stream to end at the recovered final record, and fails safely if a required
image cannot be reconstructed. It restores the dynamic strings transactionally
when reconstruction fails. The native eight-slot effect writer/reader has an
overflowing sentinel edge case; the portable reader rejects that ambiguous
layout rather than reading following records at an incorrect offset.

The production load path now honors `resume_wait_click` for original saves,
matching the portable-snapshot branch. Previously it immediately resumed the
scenario even though the importer requested a parked ADV wait.

This change does not establish full native save parity. In addition to the
explicitly reported sections above, portable sprite snapshots still lack the
native float position precision, rotation, center offsets, and render mode;
the original source rectangle is not serialized by PAL's 76-byte info call.
Imported resources start from a reconstructed cell rectangle. Font/resource
state, select interaction, fixed subsystem fields, and exact audio gain/fade
state need further integration. The importer restores current ADV strings in
live dynamic buffers; the general portable format does not yet persist those
buffers across a subsequent save and restart. No native writer mirror blocks
were fabricated or changed.

Validation: `cargo check -p pal-vm --no-default-features --lib` passed; log at
`target/reverse-audit/native-save/compile.log`. No regression tests or synthetic
stream diagnostics were added, as requested. No original-engine save fixture
was available under the inspected local Documents roots, so reconstruction has
not been exercised against an original native save. This compile result is
not proof of original-engine acceptance or visual parity.
