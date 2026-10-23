# Runtime gap audit

Audit date: 2026-10-23. This is a read-only dispatch audit plus the bounded BGV gain correction recorded below. It is not a claim that the engine port is complete.

## Counting rules

A coordinate is the actual `category:index`, not a function name. Category 6 and 13 return before the global name-alias match. Alias handling was checked before assessing category fallbacks. A `Blocked` signature means insufficient signature verification, not a missing runtime implementation. Conversely, a `Verified` signature does not establish runtime fidelity.

The initial source snapshot contained **55 constant-return/no-functional-state branches** enumerated below. Two (`8:51`, `8:52`) have no opcode-catalog entry. Three more (`8:50`, `8:53`, `18:15`) are kept separate as intentional debugger/host compatibility or an acknowledged compatibility constant. This leaves **50 catalogued candidates needing behavioral verification or implementation**, not 50 proven native features: unnamed slots and old release aliases still require checking against a particular native table. Native validity and signature counts must not be inferred from this branch count.

The BGV correction in this audit removes `13:21` and `13:22` from that candidate set. Other agents are editing this repository concurrently; these numbers describe the inspected starting snapshot and are not a stable completion metric. The list also excludes nonconstant partial handlers, missing dispatch cases, and codec features, so it is a lower-bound inventory rather than a complete remaining-work count.

## Constant-return branch inventory

The call counts below are static occurrences in `target/reverse-audit/current-extcall-report.json`, not measured execution frequency. Names are the current opcode catalog and may be stale across engine revisions.

| Coordinate | Catalog name | Static occurrences | Audit classification |
| --- | --- | ---: | --- |
| `2:48` | `voice_play` | 0 | Consumes arguments/returns a constant; behavior unresolved |
| `2:49` | `voice_stop` | 0 | Consumes arguments/returns a constant; behavior unresolved |
| `2:52` | `set_voice_info` | 0 | Consumes arguments/returns a constant; behavior unresolved |
| `2:56` | `bgv_play` | 0 | Consumes arguments/returns a constant; behavior unresolved |
| `2:57` | `bgv_stop` | 0 | Consumes arguments/returns a constant; behavior unresolved |
| `2:66` | `voice_wait` | 0 | Consumes arguments/returns a constant; behavior unresolved |
| `2:67` | `bgv_pause` | 0 | Consumes arguments/returns a constant; behavior unresolved |
| `2:73` | `voice_call` | 0 | Consumes arguments/returns a constant; behavior unresolved |
| `2:74` | `voice_call_clear` | 0 | Consumes arguments/returns a constant; behavior unresolved |
| `3:12` | `set_filter` | 235 | Consumes arguments/returns a constant; behavior unresolved |
| `4:16` | `1_get_bgm_status` | 0 | Consumes arguments/returns a constant; behavior unresolved |
| `4:17` | `1_get_bgm_pos` | 0 | Consumes arguments/returns a constant; behavior unresolved |
| `4:18` | `get_bgm_ch` | 0 | Consumes arguments/returns a constant; behavior unresolved |
| `8:42` | `set_window_text` | 0 | Consumes arguments/returns a constant; behavior unresolved |
| `8:43` | `(unnamed)` | 0 | Consumes arguments/returns a constant; behavior unresolved |
| `8:44` | `(unnamed)` | 0 | Consumes arguments/returns a constant; behavior unresolved |
| `8:45` | `(unnamed)` | 0 | Consumes arguments/returns a constant; behavior unresolved |
| `8:46` | `(unnamed)` | 0 | Consumes arguments/returns a constant; behavior unresolved |
| `8:47` | `(unnamed)` | 0 | Consumes arguments/returns a constant; behavior unresolved |
| `8:48` | `(unnamed)` | 0 | Consumes arguments/returns a constant; behavior unresolved |
| `8:49` | `(unnamed)` | 0 | Consumes arguments/returns a constant; behavior unresolved |
| `8:50` | `debug_break` | 0 | Intentional debugger/host compatibility; separate from missing functionality |
| `8:51` | `(not catalogued)` | 0 | No catalog entry; exclude from native-interface totals |
| `8:52` | `(not catalogued)` | 0 | No catalog entry; exclude from native-interface totals |
| `8:53` | `app_exec` | 0 | Intentional debugger/host compatibility; separate from missing functionality |
| `8:54` | `is_playername` | 0 | Consumes arguments/returns a constant; behavior unresolved |
| `8:55` | `(unnamed)` | 0 | Consumes arguments/returns a constant; behavior unresolved |
| `8:56` | `(unnamed)` | 0 | Consumes arguments/returns a constant; behavior unresolved |
| `8:57` | `(unnamed)` | 0 | Consumes arguments/returns a constant; behavior unresolved |
| `8:58` | `(unnamed)` | 0 | Consumes arguments/returns a constant; behavior unresolved |
| `9:11` | `(unnamed)` | 0 | Consumes arguments/returns a constant; behavior unresolved |
| `9:15` | `load_font` | 1 | Consumes arguments/returns a constant; behavior unresolved |
| `9:18` | `key_canncel` | 7 | Consumes arguments/returns a constant; behavior unresolved |
| `9:32` | `get_pull_key` | 0 | Consumes arguments/returns a constant; behavior unresolved |
| `9:33` | `get_on_key` | 0 | Consumes arguments/returns a constant; behavior unresolved |
| `9:34` | `get_push_key` | 0 | Consumes arguments/returns a constant; behavior unresolved |
| `9:41` | `get_monitor_size` | 0 | Consumes arguments/returns a constant; behavior unresolved |
| `9:42` | `(unnamed)` | 0 | Consumes arguments/returns a constant; behavior unresolved |
| `9:43` | `get_system_metrics` | 0 | Consumes arguments/returns a constant; behavior unresolved |
| `9:45` | `set_allmosaicthumbnail` | 0 | Consumes arguments/returns a constant; behavior unresolved |
| `9:54` | `get_async_key` | 0 | Consumes arguments/returns a constant; behavior unresolved |
| `9:57` | `history_skip` | 0 | Consumes arguments/returns a constant; behavior unresolved |
| `13:4` | `set_voice_info` | 4 | Consumes arguments/returns a constant; behavior unresolved |
| `13:19` | `bgv_pause` | 2 | Consumes arguments/returns a constant; behavior unresolved |
| `13:21` | `set_bgv_volume` | 3 | Corrected in this audit: cached BGV gain and group-2 volume |
| `13:22` | `get_bgv_volume` | 1 | Corrected in this audit: cached BGV gain and group-2 volume |
| `13:23` | `set_bgv_auto_volume` | 0 | Consumes arguments/returns a constant; behavior unresolved |
| `13:25` | `voice_call` | 0 | Consumes arguments/returns a constant; behavior unresolved |
| `13:26` | `voice_call_clear` | 0 | Consumes arguments/returns a constant; behavior unresolved |
| `14:17` | `history_set_face_call` | 0 | Consumes arguments/returns a constant; behavior unresolved |
| `14:18` | `history_set_face_sound` | 0 | Consumes arguments/returns a constant; behavior unresolved |
| `14:19` | `history_set_face_sound_release` | 0 | Consumes arguments/returns a constant; behavior unresolved |
| `15:2` | `set_window_text` | 133 | Consumes arguments/returns a constant; behavior unresolved |
| `15:4` | `(unnamed)` | 29 | Consumes arguments/returns a constant; behavior unresolved |
| `18:15` | `(unnamed)` | 2 | Acknowledged startup compatibility constant; native state condition not modeled |

The initial candidate list has nine game-used coordinates after excluding `18:15`: `3:12`, `9:15`, `9:18`, `13:4`, `13:19`, `13:21`, `13:22`, `15:2`, `15:4`. Seven remain in that initial list after the two BGV volume fixes, before accounting for other agents' changes. This does **not** mean seven remaining interfaces overall.

## Highest-priority confirmed behavioral gaps

1. **`3:12 set_filter`**, 235 static occurrences. `Game.sqlite` `sub_4275F0` consumes slot/mode, maps mode through `dword_49C320`, changes PAL sprite options, and accumulates/reset wrapper filter bits. The starting Rust branch consumed two values and did nothing. Root agent owns its correction.
2. **`9:18 input_key_cancel`**, seven occurrences. Native `sub_437E70` stores keyboard/mouse masks at context offsets 804236/804240 and calls `PalInputKeyCancel`; starting Rust discarded both values. **`9:35 input_clear`**, another seven occurrences, is an additional wrong implementation outside the constant-return count: native `sub_437890` calls `PalInputClear`, `PalInputKeyRepeatClear`, clears context+4192, and returns 1; Rust returned the PAL clock.
3. **BGV playback is still routed as ADV voice**: `13:8` calls `ext_voice_play`, opens group 1, and shares `(13, slot)` with ADV voices. `13:9` uses the same map with group 6. Native BGV owns eight separate lanes in group 2. Group-volume setters alone do not fix that routing. `13:19 bgv_pause` is also a one-argument no-op although native `sub_4431D0` consumes slot/pause/fade (three), expands -1 to eight BGV slots, and calls `PalSoundPause` on changed lanes. `13:10 bgv_enable` consumes one value in Rust, but native `sub_4432E0` consumes slot/enabled and writes per-lane state. These need a coordinated lane migration rather than isolated aliases.

## Nonconstant partial implementations

- **Tips `2:36..46`** have command/data handlers, but the `<dNUMBER>` record producer, glyph colors, hover/completion callback chain, and serialization remain absent. Normal record queries therefore see empty records; context selectors added earlier are real implemented queries. See `runtime/tips.rs` and `softpal-runtime-audit.md`.
- **History `14:12`** only stores its resource argument; native voice-icon drawing is absent. `14:17..19` are already in the no-op list. History wrapping still lacks native line-head prohibitions and overflow ellipsis.
- **Text `2:25`** records timing/dirty state without the native temporary check sprite. `2:28` writes one active color pair instead of the native 16-entry color table. `2:18/19/29/33` merely update an event timestamp; their advertised text effects need native verification.
- **Font `9:20`** only sets the extended-font loaded latch rather than replacing the requested bitmap resource. `9:16` correspondingly clears the latch rather than modeling all native font lifetime behavior.
- **Action `17:6/7/10/11/14/15/17/20`** schedule an action timer/ID but do not apply their native visual timeline commands. `17:5/8/29` have additional position/alpha work and must not be lumped in as empty handlers.
- **Sprite `3:40 sp_paint`** consumes six arguments but only clears cached save drawings; its native paint operation is absent at the inspected snapshot. Root owns this correction.
- **Save/load `10:0/1`** support portable snapshots and native-prefix resume; full native sprite/history/window/sound image import and native-compatible mirrored save blocks remain incomplete.
- **Movies** have WMV/MPEG decoders; MP4 is explicitly rejected. The later [MP4 audit](mp4-native-audit.md) corrects the former unsupported assumption that this was a required native codec feature. It is not an additional unique extcall coordinate.
- **Native host errors** have fatal-frame callbacks but not every recovered native error dialog call site. This is not evidence of an absent generic messagebox opcode.

## Deliberate portable behavior

`18:1 app_exec` resolves/logs the target but never launches host applications, as repository policy requires. `18:10 check_disc` uses the native debug-success compatibility path rather than scanning physical CD-ROM drives. Neither is a request to enable host execution or introduce an optical-drive requirement.

## BGV gain correction completed here

Native evidence: `sub_443150` stores `100 * percent` at context+659616 and applies it through the mute latch to PAL sound group 2; `sub_443100` returns the cached percentage, regardless of mute; `sub_443080` preserves that cache while applying zero or the cached gain. The Rust handlers now implement these operations at `13:21/22/20` and the text aliases `2:69/70/68`, using one group-2 apply helper. Changes made while muted remain cached and unmuting applies the new gain. ADV group 1 is untouched.

Validation command: `cargo test -p pal-vm --lib bgv_volume_dispatch`. The added regression test checks actual public dispatch ordering, parameter-stack preservation, muted updates, cached getter output, aliases, and independent group gains with the audio backend disabled. Playback lane routing remains the separate issue above; this test does not claim audible BGV playback is fixed.

At handoff, this focused test has not completed: concurrent work initially lacked the new history module symbols, and the latest attempt lacked `sprite/filters.rs` and `apply_surface_option`. Re-run after those parallel edits finish; do not report this new test as passing before then.

## Follow-up BGV lane correction

The playback/stop/pause group-routing gap identified above is now addressed in `bgv-playback.md`; the original branch inventory is retained as an audit snapshot. BGV per-lane auto ducking/enable and snapshot restoration remain incomplete. The four focused BGV tests subsequently passed after parallel modules became available.

## Follow-up native-helper contract correction

`native-helper-contracts.md` supersedes the initial assessment of `15:2`, `15:4`, and `13:4`: the window-title helper now captures its release-mode savepoint and pops two; the supposed overlay is native debug printing and must pop eight even when release-mode output is intentionally disabled; voice-info registration now consumes four and preserves its records, with downstream use still partial. Shared history/input signatures and contradictory observed counts were also corrected. The initial constant-return counts are historical, not a current completion count.

## Current correction index

The historical inventory above is superseded for implemented areas by [port-corrections.md](port-corrections.md): sprite filters/paint, font resources, input control, Tips producer/callback/save, frozen history, independent BGV playback/save, voice registry playback, and native helper stack contracts. It must not be quoted as a current missing-interface count.
