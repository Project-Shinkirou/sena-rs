# Native voice MIX playback

Evidence: `reverse/Game.sqlite`, IDA decompilation of Game.exe, inspected 2026-10-23.

- `sub_442C10` probes `<voice>.mix` before loading a single sound. When present, it releases voice records 0–7 (100-byte stride), reads at most eight members, and loads each into its own PAL sound handle. The incoming requested slot does not select the MIX destination. Slot 8 is separate.
- The native reader consumes 64 bytes per member and copies the low bytes of its 16-bit character cells. The portable parser now accepts null-padded 32-code-unit UTF-16LE records in addition to its existing 32-byte record format. ASCII resource names follow the observed native conversion; behavior for non-ASCII wide names remains unverified.
- Each member calls `sub_4425F0` with its own filename unless flags contain `0x40` (per-character volume bypass), and `sub_442550` / `sub_442430` unless flags contain `0x80` (autopan bypass). All handles are loaded before the playback loop starts them.
- `sub_43C2C0` releases all eight ADV voice lanes before playing a new text voice. Retaining members 1–7 from a previous MIX when the next line is a single voice was incorrect.
- `sub_443800` takes an individual slot or `-1` (mask `0xFF`). Keeping each MIX member in the existing category-13 slot map lets existing voice waits, auto-advance, stop-all, and backlog replay account for each member. An explicit individual-slot wait remains individual, matching native.

The runtime preserves the old PCM-summing decoder for callers outside the category-13 voice path. Tests use generated WAVs in a temporary directory and a disabled audio device, verifying distinct handles, the eight-member bound, independent slot 8, member volume/mute/pan, both bypass flags, next-line replacement and stop-all. Audible playback synchronization was not exercised. The subsequent fourth-argument fade correction is documented in `RUST_VOICE_FADE_FIX.md`, and the native prefix registry consumer and index-7 enable gate are documented in `voice-info-consumer.md`.
