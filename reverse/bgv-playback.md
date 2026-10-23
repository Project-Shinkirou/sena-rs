# Independent BGV playback

Date: 2026-10-23.

## Native contract

`Game.sqlite` 0x4435F0 consumes `(slot, resource, flags, fade_ms)` in pop order. Slot -1 selects the first free BGV lane in 0..7, falling back to lane 0 when full. Replacing a lane releases its previous sound. The ordinary sound loader 0x4024D0 opens group 2; the ADV MIX playlist expansion and per-character voice gain/autopan are not part of this path. Flag 1 loops; flag 4 fades in unless the native skip mask suppresses fading. The return value is the selected lane even if the resource fails to load.

0x4434E0 consumes `(slot, fade_ms)`. Only -1 selects all eight BGV lanes. Muted or skipped playback stops immediately; otherwise the native sound receives a fade-out request and its wrapper is released after completion. 0x4431D0 consumes `(slot, paused, fade_ms)` and applies pause/resume only to existing lanes whose state changes.

## Rust changes

`runtime/bgv.rs` keeps BGV handles under the internal map namespace `0x800D`, distinct from external category-13 ADV handles. This value is not an extcall category. Playback uses `PalSoundGroup::GROUP2`, eight lanes, ordinary audio resource lookup without MIX fallback, no ADV per-character gain, and no voice autopan. `13:8/9/19` and text aliases `2:56/57/67` now share the same implementation.

BGV gain and mute (`13:20/21/22`, aliases `2:68/69/70`) now reach playing group-2 channels. An ADV stop or the next ADV line cannot release BGV, and a BGV stop cannot release ADV voices. Fade-out keeps the handle until its PAL-clock deadline, with wrapping clock arithmetic; pause/resume preserves the BGV lane. A per-frame cleanup hook runs even while the script is parked waiting for input. Audio APIs accept explicit stop/pause fade durations through Kira tweens.

The six recovered signature contracts and their category-2 aliases are now explicit in `extsig.rs`; observed counts agree. The old report's zero argument entries came from prior signature fallback and are not native evidence. The zero-argument BGV volume getter is corrected from its stale observed count of two.

## Validation and limits

`cargo test -p pal-vm --lib bgv_` exercises four tests: independent eight-lane allocation and replacement, reciprocal ADV/BGV stop isolation, aliases and group gain/mute with character mute excluded, pause stack discipline, and fade deadlines across the u32 clock wrap. Fixtures use only uniquely named temporary directories. The audio device is disabled, so these are handle/state/dispatch tests; they do not verify perceived timing or the exact native fade curve. `cargo test -p pal-script` validates the shared signature catalog.

BGV lane names, looping/paused state, and remaining stop deadlines now persist in portable snapshot version 10; see [bgv-snapshot.md](bgv-snapshot.md). Replay occurs at the next VM frame, including while parked on an ADV wait. Playback offsets and instantaneous fade-envelope gain are not serialized; native save sound wrappers also remain unmapped. The native per-lane enable/automatic ducking path (`13:10/23`, 0x4432E0/0x4433A0 and frame helper 0x445560) remains incomplete. The play fade gate currently uses the portable skip-state interpretation rather than reproducing every native 0x35 bit combination. Bounds checks reject invalid lanes instead of reproducing native unchecked access. These limits prevent claiming full BGV or full-engine parity.
