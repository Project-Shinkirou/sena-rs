# Native voice-info consumer

Date: 2026-10-23. Evidence is the read-only Game.exe IDA export in `Game.sqlite`, the preserved C excerpts under `evidence/runtime-audit/`, and Koikake raw bytecode disassembly in `docs/dis.txt`.

## Corrected gate contract

Category 13 index 7 consumes one integer and controls the voice-info registry. Its historical `VoicePlayFade` label was incorrect. `sub_444190` writes main-context offset 660028. The playback wrapper `sub_4447B0` passes main-context + 4 * 163816 to `sub_442C10`; `sub_4425F0` reads that voice-context + 4764. These identify the exact same dword. Koikake pushes 1 at 0x38B58 and calls this gate at 0x38B60 before registering voice entries. Rust now names the operation `voice_info_enable`. Voice fades remain the independent fourth `voice_play` argument, guarded by flag 4.

## Recovered matching and gain

`set_voice_info(slot, name, volume, enabled)` retains 32 numbered records and the slot -1 fallback. A name resource of -1 preserves the previous prefix. Playback follows `sub_4425F0`:

- Disabled gate writes channel volume 10000 (100%), bypassing all registered percentages and mutes.
- Enabled gate scans registered slots 0 through 31 in ascending numeric order. Matching is case sensitive. The first prefix wins, including an empty prefix, which matches every name. This is not longest-prefix matching. C-string termination is respected.
- A matched entry with nonzero `enabled` applies `volume * 100`; a disabled entry applies zero and does not continue searching.
- If no entry matches, the slot -1 record supplies volume and enabled. Without that record, the zero-initialized native fallback is silent.
- Voice flag 0x40 bypasses the entire registry and supplies unity channel gain.

These decisions execute centrally in `load_voice_audio`, so ordinary `voice_play`, ADV text voice, backlog replay, and each MIX member use the same consumer. MIX members use their own names. The registry result replaces the legacy per-character channel percentage; it is never multiplied by that percentage a second time. The global PAL voice-group volume and the fade envelope continue to apply independently.

For compatibility, a game that has never called either the registry setter or its enable gate retains the existing CSV-based character settings behavior. Once either native API is used, the recovered native gate and registry behavior takes precedence. BGV uses its separate group and lanes and does not consume this registry.

## Validation and limits

`cargo test -p pal-vm --lib voice_`: 9 passed. Focused tests cover ascending-order overlap, case sensitivity, empty prefixes, disabled records, fallback, missing fallback, both gate states, and the 0x40 bypass. Generated temporary WAV and MIX fixtures exercise actual ordinary voice dispatch, text voice, backlog replay and per-member MIX loading. Conflicting CSV percentages prove registry gain is applied only once; a 40% channel under a 50% global voice group yields 20% effective gain. Setter dispatch preserves a preceding stack sentinel. Existing MIX tests retain coverage for unregistered-game CSV fallback and autopan.

Tests use a disabled audio device and do not assert audible hardware output. PAL's portable channel-volume type clamps percentages to 0..100; out-of-range native arithmetic and overflowing or unterminated native name buffers are not emulated. The normal resource-name contract is supported, but malformed encodings and native buffer corruption are not reproduced. Registry snapshot serialization is unchanged by this work; the registry is session configuration, and native save-image inclusion remains unverified.
