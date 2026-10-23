# Newer SoftPAL implementation and validation

Updated 2026-10-23. This is a compatibility increment, not a complete-port or
native-equivalence certificate. The source game directory was read-only; IDA
used copied binaries and the executable harness used symlinked assets with a
separate writable save directory under `target/reverse-audit/totsulover-game`.

## Build selection and dispatch

The newer EXE SHA-256 is
`4778fe70629a7c0ee6cb3caa30ce572cc45440ac392ea998b406ca45ec304483`.
Its PAL DLL SHA-256 is
`744219b7c2b2ba80c5bb0440ac556201652ffbcc01d2de1211c4b576ad21db7e`.
The DLL exports 369 symbols; the EXE imports 271 PAL symbols. The registrar
contains 898 calls representing 787 distinct VM command coordinates.

A bounded PE export parser recognizes the joint presence of
`PalSystemPathInit` and `PalGetChangeWindowModeTime`. It does not execute the
DLL. That capability profile selects 215 recovered primary-opcode aliases,
the newer reviewed wrapper contracts, archive lookup precedence, sprite-cell
clamping, and effect bounds. It is verified for this fixture and is not a
universal engine-version detector.

There are 38 individually reviewed newer VM wrapper signatures. The remaining
749 coordinates have not received the same build-specific review; this count
does not mean that 749 handlers are missing. Existing legacy handlers may
implement some of them, and their applicability still needs confirmation.
Two reviewed history configuration wrappers still have no complete consumer.
The native ABI manifest separately has 29 declarations, 26 with newer-DLL
evidence. These counts refer to different interfaces and are not percentages
of engine behavior.

Configuration/no-op wrappers that do not write a VM destination now preserve
it. A native handler's integer scheduling result is not automatically copied
into the script's return slot. Important corrected coordinates include 9:63
(`is_active`), 18:127 (window-mode change time), 18:90 (eight-word debug output),
3:59 (native two-word no-op), and 2:47 (text animation mode).

## Resource loading and visible behavior

- `ARCHIVE.DAT` supports comma-separated profiles containing pipe-separated
  paths. The parser decodes the configured NLS before splitting, so Shift-JIS
  trail bytes are not mistaken for separators. Category 9 index 44 replaces
  the active profile. Newer registered paths are searched last-added-first;
  trial/patch assets therefore override base archives. Legacy ordering remains
  unchanged, including localized archive priority.
- Ordinary newer `sp_set` probes `.pgd`, `.mpg`, `.tga`, `.bmp`, `.png`, `.jpg`
  in native order and continues after a failed decoder. It appends suffixes
  even when the supplied basename contains a dot. The `.mpg` entry creates a
  real movie sprite, initially stopped, in the ordinary slot; `msp_play`
  advances its existing decoder. Replacing/clearing the slot releases it.
- JPEG and static PNG signatures decode to RGBA with bounded allocations.
  PNG expands palettes, tRNS and low-bit grayscale, preserves straight alpha,
  and strips 16-bit channels to their high bytes. JPEG covers gray, RGB and
  CMYK. Native color management, JPEG rounding and animated PNG are unverified.
- The native `<br>` text control now creates a line break, including at the
  beginning of a record. The text mode's per-pixel threshold/fade kernel is
  implemented; portable glyph geometry and the existing character-duration
  model do not establish native timing, ruby, or append behavior parity.
- Movie sprite loading accepts the existing MPEG and WMV decoder paths.
  MP4 is rejected; this release's PalVideoPlay accepts WMV/MPG. The separate
  debug PAL_d.dll H.264 branch is incomplete; see [MP4 audit](mp4-native-audit.md).

## SE playback and native audio contracts

Category 5 indexes 0..18 now use the reviewed newer dispatch shapes, including
separate prefix records, sixteen playback slots and three group selectors.
The selector mapping is `[5, 4, 6]`, not a single combined SE group.

Load/play/play-ex now distinguish filenames, slots, group selectors, loop and
wait flags, fade duration, and pending pan. Unload pops one word. A blocking
play/wait re-enters without popping arguments again. Immediate stop retains
preloaded channels, while unload releases them. Prefix matching is
case-sensitive, the last initialized match wins, and the enable word is not
invented as a mute gate for a consumer that does not consult it.

Per-slot volume/frequency overrides and mute are applied through the recovered
paths. The immediate-volume setter preserves the native logical-slot versus
encoded-handle lookup quirk; it does not silently turn into a direct setter.
Pending SE pan uses the limit shared with `voice_autopan_initialize` and is
consumed by mode bit 0x20. Frequency now changes Kira playback rate and queries
return the original sample rate when reset to zero. `PalSoundLoadEx` applies
the recovered signed-16-bit amplification/clipping before channel gain.

Remaining audio differences include decoded PCM rounding, native allocation
and load flags, raw out-of-range channel gain, worker scheduling/retirement,
save restoration of all transient SE control state, and exact device timing.
These are recorded differences, not silently verified behavior.

## End-to-end fixture

The isolated headless run reaches the original MPEG logo, trial title,
protagonist voice prompt, trial chapter selector, and the main ADV scene.
At frame 1800 it reaches a parked text command around PC `0x131C00`; the JPG
background is present. This proves a startup/story-entry path, not a route
playthrough or frame comparison against a running original engine.

The current run still reports missing `history_face_enable` (14:13),
`history_secondary_icon_enable` (14:24), and `btn_onmouse_clear` (8:26).
Their native consumers/callbacks are preserved in evidence. Merely storing
their arguments would not implement the visible/callback behavior, so these
remain explicit gaps. Native saves, full graphics modes, remaining wrappers,
and console/mobile device acceptance are tracked in [port status](port-status.md).

Reproduce after `cargo build -p pal-vm --release --bin sena`:

```sh
RUST_LOG=info target/release/sena target/reverse-audit/totsulover-game \
  --headless --no-audio --diagnostic-frames 1800 --diagnostic-frame-ms 0 \
  --fixed-timestep-ms 16 --diagnostic-auto-advance \
  --diagnostic-click-when-hit-enabled 200:140 \
  --diagnostic-click-when-hit-enabled 540:410 \
  --diagnostic-click 1200:600:155 \
  --diagnostic-png target/reverse-audit/totsulover-story-fixed.png
```

The [validation record](evidence/totsulover-validation.json) records checks,
counts, source hashes and package hashes. Build success is not device validation.
