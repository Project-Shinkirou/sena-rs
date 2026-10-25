# BGM wrapper byte ranges and fade durations

The former implementation incorrectly called the last BGM argument user volume,
treated the two preceding arguments as sample loop points, and discarded fade
time. The reviewed wrappers instead pass the last three DWORDs directly in a
native `PalSoundLoadRange32` descriptor.

| VM call | Pop order | Native operation |
| --- | --- | --- |
| 4:0 | slot, filename, flags, fade units, initial bytes, loop start bytes, loop end bytes | Load group 3 and PlayFade |
| 4:1 | slot, fade milliseconds | StopFade; -1 selects both script records |
| 4:3 | none | Return configured BGM percent |
| 4:9 | slot, filename, initial bytes, loop start bytes, loop end bytes | Preload group 3 |
| 4:10 | slot, flags, fade units | PlayFade the loaded handle |

Play enables the range with `flags & 0x10`. Preload enables it when any of the
three supplied offsets is nonzero. A zero end is resolved to the native decoded
byte length by the PAL loader. The wrapper does not read BGM.CSV or convert these
words to samples. Its loader delegate only probes sound suffixes and forwards
the pointer. Any script-side table lookup must therefore happen before this
interface. The previous implicit BGM.CSV fallback in these VM handlers has been
removed; a CSV column is not silently applied a second time.

Play and Play2 use wrapping unsigned arithmetic:

```
duration = scheduling_state != 0 ? (fade_units >> 4) : fade_units;
PalSoundPlayFade(handle, flags & 1, duration * (flags & 4));
```

Thus the multiplier is literally four when the fade bit is set. Stop's duration
has the unsigned scheduling shift but no multiplication. Repeating a nonzero
StopFade while fade-state 2 is already active does not restart the fade.

Reference/newer Play, Load, and Play2 write the selected script slot to the VM
destination. Koikake's wrappers preserve it. Stop preserves the destination in
all three. Invalid slots take native control-return 0; the portable VM still
needs its general control-return failure adapter to distinguish this from a
successful Continue.

For automatic slot -1, the native wrapper preserves the previous script record
in the second record while one bank slot is free. With no free bank entry it
releases group 3 first. The newer build additionally clears stale nonreserved
script records before checking availability. An ordinary specified-slot Play
does not first release its old handle, and does not contain the portable
same-filename reuse shortcut.

The loader scheduling gates differ by build. Koikake skips loading for state
bit `0x10`; reference skips for `0x30`. The newer loader also checks `0x30`, but
group 3 calls `PalSoundLoadSetReserved` instead of opening the resource. That
export only reserves one of two unloaded/unreserved records. It does not retain
the filename, range pointer or a deferred decoder job. Reservations exclude the
entry from allocation, are visible as raw selector 17 and active selector 18,
and are cleared by newer Stop, Release and group Release. The portable BGM
adapter now follows these gates and retains the EXE's script description
separately. Preload records keep their `0x20000000` script flag during newer
automatic-slot cleanup. No automatic decode-on-unskip behavior is invented.

## Addresses and source evidence

- Totsulover EXE SHA-256 `4778fe70629a7c0ee6cb3caa30ce572cc45440ac392ea998b406ca45ec304483`: Play `0x41DEB0`, Stop `0x41DD60`, getter `0x41DD10`, Load `0x41E360`, Play2 `0x41E220`; descriptor at 92-byte record offsets 64, 68, 72, 76. Loader `0x408CE0` forwards it to PAL. Evidence: `evidence/bgm-totsulover-{wrappers,getters}.json`, `evidence/totsulover-se-prefix-volume.json`.
- Reference EXE SHA-256 `219395e636f8fb8eb1bc7790345b0de5294671b33f8542edf074b333b0722e8e`: Play `0x40C4B0`, Stop `0x40C380`, Load `0x40C8D0`, Play2 `0x40C7A0`; descriptor at 124-byte record offsets 96, 100, 104, 108. Evidence: `evidence/bgm-reference-wrappers.json`.
- Reference loader `0x4024D0` and its scheduling mask are recorded in `evidence/bgm-reference-loader.json`. Newer PAL `PalSoundLoadSetReserved` `0x100B2DA0` and `PalSoundSetReserved` `0x100B2D60` are in `evidence/audio-totsulover-exports.json`, SHA-256 `744219b7c2b2ba80c5bb0440ac556201652ffbcc01d2de1211c4b576ad21db7e`.
- Koikake EXE SHA-256 `93f228aa413aece0f047b27ddb1ea26d4e50316304e6c4029eae4a1a844f437b`: Play `0x40D5B0`, Stop `0x40D450`, getter `0x40D400`, Load `0x40D8B0`, Play2 `0x40D7C0`; descriptor at 92-byte record offsets 64, 68, 72, 76. Auto-slot helper `0x40DE20`, loader `0x4047B0`. Evidence: `evidence/bgm-koikake-{wrappers,getters,loader}.json`; category table `0x478548` is recorded separately.

Machine-readable wrapper declarations are in
`evidence/bgm-reviewed-wrappers.json` and incorporated into
`contracts/audio_video.json` before generating the registered dispatch catalog.

## Portable PCM mapping and remaining boundaries

`audio/source_format.rs` retains original channels, sample rate, bit width,
block alignment and decoded byte length. WAV uses its fmt/data chunks; Ogg
retains 16-bit PCM metadata and the native byte-offset shift by two for seeks;
WMA retains decoder output channels. A mixed playlist has no single native
source-buffer byte layout and does not pretend to have one.

The group-3 loader range now sets initial playback position and loop bounds.
Ordinary buffer byte cursors derive from source frame position and block
alignment; group-3 absolute source position is available when its seek stride
matches that alignment. The actual DirectSound ring cursor and ring read/write
state remain unsupported. The PCM source layout also supplies the ordinary
buffer-size and WAV/Ogg format selectors. Native resource addresses remain
explicit errors, except a copied buffer's known cleared resource pointer is zero.

Compressed WAVE byte seeking, invalid/native out-of-bounds seeks, extending an
already-playing nonlooping range, native worker
race/acknowledgement timing and hardware presentation-clock comparison remain
incomplete. Portable saves retain loop sample bounds but do not yet retain the
native initial byte offset or exact playing cursor. No regression tests were
added or run for this follow-up at the user's request; compile validation and
actual isolated-game execution are separate evidence.
