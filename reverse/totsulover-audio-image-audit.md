# Newer SoftPAL audio and generic image loading

Reverse-only audit, 2026-10-23. The copied EXE has SHA-256
`4778fe70629a7c0ee6cb3caa30ce572cc45440ac392ea998b406ca45ec304483`.
All addresses below belong to that build. Original game files were read-only.

## Audio loading registers and PAL groups

The compiler-generated helper `0x408CE0` uses EDX for the PAL group, ECX for
the narrow basename, XMM3 for a gain multiplier, and one stack word for load
flags. It is an internal register-call function, not a PAL export. Do not infer
its argument list from an earlier decompilation showing only `sub_408CE0(0)`.

Actual SE calls establish the mapping:

- `5:2` at call `0x462BEC`: EDX reads `0x4D2534[group_selector]`; ECX addresses
  the resolved filename; XMM3 is zero; the stack word is zero. The selector is
  the fifth popped VM argument.
- `5:0` at call `0x463045`: the same group table is used for its third popped
  argument. XMM3 is the per-slot volume override divided by 100 when override
  flag bit 0 is set, otherwise zero.
- `5:17` at `0x463360` stores channel volume/frequency overrides and forwards the
  same group selector, with a gain derived from its volume argument.

The three table words are `[5, 4, 6]`; these are native PAL group identifiers.
The loader probes sound suffixes and calls `PalSoundLoad`. For PAL group 4 only,
a gain strictly greater than 1 selects `PalSoundLoadEx`. Under the native skip
mask `0x30`, only group 3 uses reserved loading; other groups return zero.

Evidence: [register call sites and helper body](evidence/totsulover-audio-image-consumers.json),
[raw group/suffix table](evidence/totsulover-audio-image-tables.json).

## SE signatures and state semantics

The argument order in this table is stack-pop order, matching the native debug
labels. All arguments occupy VM 32-bit words.

| Coordinate | Native name | Arguments | Destination behavior |
|---|---|---|---|
| 5:9 | get_se_ex_volume | prefix_slot | Writes stored volume only for an initialized valid slot; otherwise preserves destination. |
| 5:10 | set_se_ex_volume | prefix_slot, volume | Preserves destination. Enables the registry record and its enable field. |
| 5:11 | se_enable | prefix_slot, enabled | Preserves destination. Stores the raw word when the record is initialized. |
| 5:12 | is_se_enable | prefix_slot | Writes raw enable, or -1 for an uninitialized valid slot; invalid slot preserves destination. |
| 5:13 | se_set_pan | position | Preserves destination; queues pan for a subsequent playback call. |
| 5:14 | se_mute | muted, group_selector | Preserves destination; valid selectors are 0..2. |
| 5:15 | se_set_freq | channel_slot, frequency, apply_now | Preserves destination. |
| 5:16 | se_get_ch_freq | channel_slot | Writes `PalSoundGetFreq` for that slot's native sound handle. |
| 5:17 | se_play_ch_ex | channel_slot, filename, mode, fade_ms, group_selector, volume, frequency | Writes the selected logical slot. |
| 5:18 | se_set_ch_volume | channel_slot, volume, apply_now | Preserves destination. |

The four prefix records are separate from the sixteen playback channels.
`5:8`'s fourth field is the same raw enable word queried by `5:12` and modified
by `5:11`; its first field is record initialization. Do not collapse those two
states. Setter behavior alone does not prove where every enable gate is used.

The per-channel override record is three words at `ctx + 159844 + 12 * slot`:
flags, volume percent, and frequency. Volume >= 0 sets flag bit 0 and stores the
volume; a negative value clears the flag and stores zero. Frequency > 0 sets
flag bit 1 and stores frequency; a nonpositive value clears the flag and stores
zero. Immediate frequency application forwards the original raw frequency to
`PalSoundSetFreq`, including zero or negative input.

The immediate-volume path has an evidenced native quirk: `0x46318B` calls
`0x461900` with the logical slot in EDX, while that helper searches the sixteen
native sound handles for equality with EDX. It is not equivalent to directly
applying volume to the selected slot's current handle. Once the helper finds a
match, it converts percent to native volume by wrapping multiplication by 100
and unsigned clamping to 10000, then applies any enabled frequency override.
Preserve this distinction in parity claims and tests.

Mute uses `(muted == 0)` as the group's enable multiplier and applies the stored
native group volume through `[5,4,6][selector]`. It does not rewrite the stored
volume setting. `se_set_pan` uses the logical screen width divided by 20 to map
positions to a pending signed pan bounded by `ctx + 159664`: below 0 selects
the negative limit, above 20 the positive limit, and 0..20 interpolate using
native integer division followed by float arithmetic. Mode bit `0x20` consumes
this pending pan and resets it to zero when playing a channel.

Some native channel setters check only `slot >= 16`, leaving negative inputs
unsafe. These bodies do not justify emulating memory corruption in Rust.
`5:16` performs no native bounds check. Such invalid input remains outside the
verified safe-input behavior.

Evidence: [setters/getters and instructions](evidence/totsulover-audio-image-consumers.json),
[override helper](evidence/totsulover-audio-image-boundaries.json),
[machine-readable signatures](evidence/totsulover-audio-image-reviewed.json).

## Generic sprite suffix dispatch

The table at `0x4EB680` contains six suffixes with associated loader pointers.
The pointers are initially zero in the PE and are assigned by initializer
`0x401300`; reading the static table alone would miss the behavior.

| Probe order | Suffix | Loader |
|---:|---|---|
| 1 | .pgd | PalSpriteLoad |
| 2 | .mpg | PalMSpriteLoad |
| 3 | .tga | PalSpriteLoad |
| 4 | .bmp | PalSpriteLoad |
| 5 | .png | PalSpriteLoad |
| 6 | .jpg | PalSpriteLoad |

`0x479900` formats `basename + suffix` on every probe and returns the first
nonzero load. There is no bare-name attempt or extension detection. On this
path an explicit `name.png` becomes `name.png.pgd`, `name.png.mpg`, and so on.
Adding a portable explicit-extension shortcut would be a deliberate extension,
not this build's native behavior.

Under skip mask `0x30`, the parallel table at `0x4EB6C0`, initialized by
`0x4012C0`, substitutes `PalSpriteInfoCreate` and `PalMSpriteInfoCreate` in the
same order. If sprite-info caching is enabled, cached metadata is tried before
those metadata loaders. A basename beginning with `#` instead creates a solid
color sprite, or metadata-only dimensions under the skip mask.

## How ordinary sp_set reaches movies

`3:2` (`sp_set`, `0x44CB40`) resolves the name through `0x47D3D0`, optionally
replaces it using a `graphic.dat` alias, then calls lock wrapper `0x479B80`,
which calls the suffix helper. The filename resolver copies/uppercases names;
it does not strip extensions. `3:3` (`sp_set_ex`) follows the same loading path.
Both therefore accept a movie basename through the `.mpg` entry and keep the
result in the ordinary sprite slot. Placement, colors, transforms, and render
priority continue through the shared sprite wrapper.

Loading through `sp_set` does not itself call `PalMSpritePlay`. The dedicated
`msp_play` handler at `0x45B0A0` accepts a logical slot and play argument,
checks `PalMSpriteCheck`, sets wrapper flag `0x400000`, saves the argument, and
calls `PalMSpritePlay`. Restoration at `0x446280` replays a movie only when that
saved flag is set. `msp_stop` at `0x45B010` checks and stops the movie in the
same ordinary slot. This evidence establishes the Game.exe dispatch boundary;
it does not independently prove every decoder-initialization side effect.

Evidence: [initializers and loader references](evidence/totsulover-audio-image-boundaries.json),
[sp_set and name resolution](evidence/totsulover-audio-image-sprites.json),
[lock wrapper and graphic alias lookup](evidence/totsulover-audio-image-load-wrapper.json),
[movie playback/restoration](evidence/totsulover-audio-image-movie-control.json).
