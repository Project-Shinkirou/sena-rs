# Native helper stack and behavior corrections

Date: 2026-10-23. Scope: runtime dispatch, shared signatures, raw script disassembly and native helper evidence. This does not establish overall engine completeness.

## Evidence hierarchy

The old `current-extcall-report.json` derives argument counts from the signature table being audited. Its zero-count entries are not independent evidence. Native stack-top decrements and raw `docs/dis.txt` pushes are the independent evidence used here. The stale `observed_pop_count` table is corrected only where the existing handler body establishes a contract; its name is not a proof of observation.

| Coordinate | Recovered count | Independent evidence | Correction |
| --- | ---: | --- | --- |
| `15:2 set_window_text` | 2 | Game 0x411360 pops two; Koikake 0x6B4D0 pushes 0, 0x6B4D8 pushes 6548, 0x6B4E0 calls | Runtime previously popped one; signature fallback zero; observed table three. All now two. Release-mode savepoint capture is implemented. |
| `15:4 debug_print` | 8 | Game 0x411090 has an eight-iteration pop loop before PalDebugIs; Koikake 0x14AC, 0x34D54 and 0x57790 each follow eight pushes | Removed incorrect three-argument baseline and content-dependent 3/8 heuristic. Renamed the misleading window-overlay signature. |
| `13:4 set_voice_info` | 4 | Game 0x443FB0 pops four; Koikake 0x38C08/0x38C28/0x38C3C/0x38C50 push four before 0x38C58 | Runtime previously popped one; fallback signature zero. Runtime now retains the native registry fields and the explicit signature consumes four. |
| `14:12 history_set` | 1 | Game 0x42BE30 pops one and copies a resource name; Koikake 0x358B4 pushes 1415 before 0x358BC | Explicit resource-string signature replaces false-zero fallback. Existing runtime consumption was already one. |
| `9:18 input_key_cancel` | 2 | Game 0x437E70 and PAL input helper audit | Existing explicit signature stays two; stale observed count three is corrected. |
| `9:32/33/34/35` | 0 | Native pull/on/push/clear wrappers have no stack decrement | Added explicit signatures for the newly corrected runtime input handlers. |
| `9:54 get_async_key` | 1 | Native GetAsyncKeyState wrapper consumes one Windows virtual key | Explicit one-argument signature and observed count. |
| `13:3 voice_get_volume` / `13:18 voice_wait` | 0 / 1 | Native 0x4445A0 / 0x443800; existing explicit signatures already correct | Corrected only the contradictory observed counts, formerly two each. |

`9:15 load_font` belongs to the concurrent font implementation and is not modified by this task.

## Savepoint behavior hidden by set_window_text

Native 0x411360 changes the host title only inside `PalDebugIs`. It then runs `AdvSetSavePoint` in ordinary release builds too, subject to the native savepoint-lock and skip-bit-0x10 gates. The portable early dispatcher now consumes both arguments and captures the resumable scene through the existing save checkpoint helper under those gates. It does not change the remembered save slot or treat the debug label as a save title. Debug host-window title formatting remains unimplemented and is explicitly marked partial.

## Debug printing is an intentional release no-op

`15:4` is not a window geometry API. The native release branch discards all eight arguments and returns 1. Rust now does precisely that stack operation independently of release detection or sentinel values. Native debug formatting/output is not implemented, so its implementation status remains Partial; the verified eight-argument decompiler contract is separate.

## Voice registry scope

`13:4` pop order is `(slot, name, volume, enabled)`. Slots 0..31 have 44-byte native records; slot -1 is the fallback record. A name argument of -1 preserves the previous prefix. Rust records those fields and bounds-checks the slot. The consumer 0x4425F0 performs name-prefix matching and uses the registry to apply channel volume. The follow-up in `voice-info-consumer.md` now consumes this registry for ordinary voice, text voice, history replay and individual MIX members. The original CSV behavior remains only for games that have never configured the registry or its enable gate. Registry serialization is outside this change.

## Validation

- `cargo test -p pal-decompiler` includes a new real instruction-stream test for all four helper calls. It verifies arguments are rendered and a preceding sentinel still reaches the following syscall, proving the corrected consumption does not steal caller stack values.
- `cargo test -p pal-script` includes explicit-vs-observed count agreement for the recovered helper/input contracts.
- Runtime tests cover the exact Koikake window-title push order, release savepoint creation, locked-checkpoint preservation, voice registry updates with name=-1, and fixed eight-pop debug printing for both release configurations.

No tests open or write the installed game directory. Native evidence is preserved in `reverse/evidence/runtime-audit/sub_411360.c`, `sub_411090.c`, `sub_443FB0.c`, `sub_4425F0.c`, and `sub_42BE30.c`.

Completed validation snapshot: pal-vm library tests 137 passed/4 ignored; pal-decompiler 33 passed; pal-script 14 passed across library and integration suites. Concurrent work may change subsequent totals. `git diff --check` passed.
