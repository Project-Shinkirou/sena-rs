# Native movie-format policy

Corrected 2026-10-23 after checking the individual DLL builds in IDA.

Game.exe `sub_42E650` uses the table at `0x49C3D0` for automatic suffix
lookup: `.wmv`, `.mpg`, `.avi`, then a null terminator. The Rust runtime
probes the supplied name followed by these suffixes, with case variants for
case-sensitive hosts. MP4 is not an automatic fallback suffix.

The three checked release DLLs do **not** accept `.mp4` in `PalVideoPlay`:

| Binary | Function | Accepted suffix branches |
|---|---|---|
| Koikake `testcase/dll/Pal.dll` | `0x100C8A50` | `.wmv`, `.mpg` |
| Reference `reverse/PAL.dll` | `0x100BACB0` | `.wmv`, `.mpg` |
| Totsulover `dll/Pal.dll` | `0x100C1200` | `.wmv`, `.mpg` |

Each returns zero for MP4 before initializing the playback graph. The WMV
branch also has a native configuration gate. See the disassembly and binary
hashes in [the MP4 audit](mp4-native-audit.md).

The earlier conclusion that MP4 was supported came from `PalVideoPlay_0` at
`0x1026BDB0` in **a different debug binary**, `PAL_d.dll`, represented by
`reverse/PAL.sqlite` (MD5 `170865af5cfcad1208b738299abb2f99`). It does recognize
`.mp4`, dispatch mode 3 and request `CLSID_CH264DecMediaObject`. However, its
H.264 graph leaves the source and audio-decoder filter slots unset before
connecting them. That is evidence of an incomplete implementation, not working
MP4 playback. The original debug binary is unavailable for a fresh execution
or disassembly check; the audit distinguishes its saved decompilation from the
release DLL evidence.

The port rejects detected MP4 with an explicit unsupported-profile error.
Container detection is retained only to explain rejection. OpenH264 is not
added under the user's condition that native MP4/H.264 playback must first be
established. This decision does not assert that every other SoftPAL build has
the same capabilities. AVI in Game's lookup table likewise does not establish
an implemented AVI decoder in this port.

MPEG playback uses `na_mpeg2_decoder`; WMV/WMA uses `wmv-decoder` from
`crates/na_wmv_player`. Both were imported from xmoezzz/siglus_rs commit
`5f358ef1360b0439e92aed2b749e8f25d8721954`. The active pal-vm movie path does not
use the old workspace `plmpeg` crate. Existing frame timing, pending ASF
payload, pause, loop, and completion fixes remain in place. Unsupported codec
variants remain explicit errors. Movie audio scheduling is a separate gap:
`VmRuntime::ext_movie_play` currently has no AudioSystem connection; having
WMA/MPEG audio decoders does not by itself provide a movie soundtrack.

Evidence: [Game lookup decompilation](evidence/runtime-audit/sub_42E650.c),
[versioned suffix inventory](evidence/movie-extensions.json), and
[MP4/H.264 investigation](mp4-native-audit.md).
