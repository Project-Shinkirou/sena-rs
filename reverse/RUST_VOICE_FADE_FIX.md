# Rust voice fade correction (2026-10-23)

Evidence: `evidence/runtime-audit/sub_442C10.c` calls `PalSoundPlayFade(handle, flags & 1, fade_ms)` when bit 4 is set, for both the ordinary voice path and all eight MIX lanes. `PAL.sqlite` contains `PalSoundPlayFade_0` at `0x10255D20`: it schedules sound state 6, saves the requested duration in sound-record dword 17, and resets the fade cursor.

Implemented in `crates/pal-vm/src/runtime.rs` and `audio.rs`:

- The fourth `voice_play` argument now reaches the audio backend. With flag 4 clear, playback remains immediate.
- Ordinary voices and each preloaded MIX member use the same `AudioSystem::play_fade` path. Character volume, mute, group volume, and autopan continue to compose with the fade envelope.
- Kira receives the target gain before playback is scheduled. This removes the former initial full-volume buffer followed by an asynchronous set-volume command.
- Non-positive durations use immediate playback. The fade envelope is independent of channel-volume updates.

Verification: `cargo test -p pal-vm --lib voice_` passed 6 tests. The new test renders the real Kira `Sound` with `process_one`, verifies increasing sample gain over the requested second, checks the final -6 dB gain, and checks immediate zero-duration playback. The existing MIX test confirms separate native lanes and preserved character settings.

Limit: the original PAL per-tick interpolation curve has not been recovered. The implementation uses Kira's linear tween of its independent fade envelope; sample-for-sample native curve identity is not claimed. The former category-13 index 7 `voice_play_fade_ms` interpretation is superseded: native 0x444190 controls the voice-info registry enable gate, as documented in `voice-info-consumer.md`. Only the fourth argument of `voice_play` supplies this fade duration. Fade-out/stop interpolation is outside this fix.
