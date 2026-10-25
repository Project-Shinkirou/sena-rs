# Confirmed implementation status

Updated 2026-10-24. Counts below distinguish reviewed contracts, implemented dispatch paths, and observed execution. They are not a compatibility percentage. No regression tests were added or run for this batch.

Accepted contracts are retained in the [English ADR index](adr/README.md).
[ADR-0002](adr/0002-preserve-native-sprite-size.md) removes the standing-art
height-fit approximation and duplicate rectangle projection. Both game profiles
have inspected portrait captures in the [targeted validation](evidence/standing-size-validation.json).
Previously saved flattened transforms can retain the old error. The longer newer
run also reproduces the baseline point-resolution fault at PC `0x10D8FC`; this
separate VM issue remains open.

## The previously identified twelve branches

| Coordinate | Implemented behavior | Native handler |
| --- | --- | --- |
| `8:42` | One-argument wrapping invalid-time gate, expired before input polling | `0x426af0` |
| `8:43` | Raw run-skip latch gates both button and shortcut polling | `0x426b60` |
| `8:44` | Group lock query with the native invalid-group result | `0x426bc0` |
| `8:45` | Five-argument attachment/detachment and ordered per-frame child transforms; preserves child base color | `0x426d60` |
| `8:46` | Pending reaction query, all-group mode, and invalid-group VM yield | `0x426f50` |
| `8:47` | Live PAL-style hit query independent of Game group locks | `0x427050` |
| `8:48` | Global invalid-time, initialization, lock, disabled-entry and missing-wrapper status bits | `0x427150` |
| `14:13` | Master/capture gates, offsets, per-record capture, and actual history face buttons | `0x455af0` |
| `14:17` | Face-button click callback used by the layout consumer | `0x457de0` |
| `14:18` | Group-5 sound loading and ownership; native field has no playback reader | `0x457e40` |
| `14:19` | Zero-argument stop/release, preserving the VM stack and destination | `0x457ef0` |
| `14:24` | Secondary icon gate/offsets, recorded checkpoint IDs, button creation and callback routing | `0x458380` |

These twelve are no longer constant-return or missing dispatch branches. This is an implementation statement, not proof of complete equivalence for every branch. Supporting history wrappers `14:12,14..16,21..23`, `10:37..39`, `9:57`, and `18:54` now have explicit reviewed contracts and consumers. History checkpoints use portable v15 files in `save/sena_rs/history`, not native binary images. Native checkpoint-file interchange is not established.

Definitions: [totsulover-twelve-reviewed.json](evidence/totsulover-twelve-reviewed.json). Consumers and their machine code are retained in the referenced evidence files. Existing native PAL declarations cover the button, sprite and sound calls; the new signatures describe the distinct Game.exe VM wrappers.

## Remaining boundaries

- The newer executable has 787 registered coordinates; 94 are individually reviewed, leaving **693 unreviewed for that build**. This is not a missing-handler count. There is still no verified total of all missing or partial behavior across the engine.
- Follow-up IDA review confirms newer `3:4` (`0x44BBC0`) consumes two arguments (slot and placement mode), but the current fallback still consumes four coordinates. Correcting that dispatch and its ten-/twenty-step version mode remains necessary. Evidence: [wrapper](evidence/totsulover-sprite-packed-position.json), [version flag](evidence/totsulover-sprite-placement-config.json).
- Full wrapper animation/transform-bank parity, native save interchange, decoder corner cases, exact rendering kernels and platform hardware acceptance remain in [port-status.md](port-status.md).
- History icon callbacks, checkpoint replay and v15 persistence compile, but this batch's real game runs do not exercise all of those branches. An imported native save does not reconstruct the newer icon/checkpoint metadata.
- The newer trial now displays the title buttons and enters the main scenario through GAME START, the voice confirmation, and chapter selection. `3:48` previously ignored packed button references, so the script skipped its fade actions; `3:25` also ignored their position deltas. `6:3` incorrectly created a wait task and could miss a one-frame click. These paths now follow the recorded native resolver and VM slice-ending behavior. This is a short scenario run, not a complete route comparison. No game-private variable is forced by the implementation.

The 921 exported-name/build audit rows count boundary evidence, not completed port behavior. Earlier implemented changes beyond the startup-capable version include button completion/sounds, child transforms, BGM range/clock semantics, native save reconstruction and the UI/history work above.
