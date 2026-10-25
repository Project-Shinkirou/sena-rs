# Native MP4 and H.264 audit

Audit date: 2026-10-23. This audit does **not** establish working native MP4 or H.264 playback. All three inspected release DLLs reject `.mp4` before constructing a playback graph. A separate historical debug DLL contains an explicit H.264 graph builder, but its recovered code leaves essential graph pointers unset. That branch is insufficient grounds for enabling an H.264 decoder as native-compatible behavior.

## Build identity and release behavior

| Binary | SHA-256 | `PalVideoPlay` |
| --- | --- | --- |
| `reverse/PAL.dll` | `e676972ddaab2d45fc82210bb89143923f10d1053492a89e50571dbcec54840a` | `0x100BACB0` |
| `testcase/dll/Pal.dll` | `2a43bc4408aff086c18dc73ccd2d35be4f8a56cf8b3e82da7b75f80ffe61625e` | `0x100C8A50` |
| Totsulover `Pal.dll` | `744219b7c2b2ba80c5bb0440ac556201652ffbcc01d2de1211c4b576ad21db7e` | `0x100C1200` |

Fresh IDA exports from copied databases include both pseudocode and disassembly in `evidence/mp4-reference-play.json`, `evidence/mp4-koikake-play.json`, and `evidence/mp4-totsulover-play.json`. The corresponding binary hashes were computed from the release files, rather than inferred from database names.

Each implementation accepts `.wmv` as mode 2 and `.mpg` as mode 1. Koikake and Totsulover additionally gate the WMV branch on an engine setting. Failure of both suffix comparisons returns EAX zero. There is no MP4 branch in these functions. For Totsulover, the `.mpg` comparison is at `0x100C1271`, its failure jumps at `0x100C1278` to the zero return at `0x100C1332`, and graph initialization occurs only later at `0x100C129A`.

Totsulover graph initialization `0x100C0910` dispatches only modes 1 and 2, to `0x100BF020` and `0x100BF8B0`. These are explicit DirectShow graphs, not evidence that arbitrary installed decoders make arbitrary file suffixes work. The MPEG path creates the MPEG-1 splitter, MPEG video decoder, and MPEG audio decoder. The WMV path creates the ASF reader and DMO wrappers for WMA and WMV decoders. See `evidence/mp4-totsulover-graph.json` and `evidence/mp4-totsulover-loaders.json`. These findings do not prove the complete codec/profile support of every system filter.

`evidence/mp4-native-contract.json` records GUID bytes read directly from the newer release, including:

- MPEG-1 splitter: `336475d0-942a-11ce-a870-00aa002feab5`, at `0x100F3B44`.
- MPEG video decoder: `feb50740-7bef-11ce-9bd9-0000e202599c`, at `0x100F3B54`.
- MPEG audio decoder: `4a2286e0-7bef-11ce-9bd9-0000e202599c`, at `0x100F3B64`.
- ASF reader: `187463a0-5bb7-11d3-acbe-0080c75e246e`, at `0x100F3BA4`.
- DMO wrapper filter: `94297043-bd82-4dfd-b0de-8177739c6d20`, at `0x10116C5C`.

## Historical debug branch

The often-cited `PalVideoPlay_0` at `0x1026BDB0` belongs to **PAL_d.dll**, represented by `reverse/PAL.sqlite`, with recorded binary MD5 `170865af5cfcad1208b738299abb2f99`. It is not the `reverse/PAL.dll` release above. The original debug binary was unavailable at the database's recorded location, so this part of the audit uses historical decompilation rather than newly verified machine instructions. `evidence/mp4-debug-graph.json` preserves that limitation, the database hash, and the relevant function bodies.

The debug function recognizes `.mp4`, assigns mode 3, and calls `0x10266CB0`. That initializer dispatches mode 3 to `0x10267440`. The latter explicitly initializes a DMO wrapper with the object at `0x10376E74`, identified by a native diagnostic as `CLSID_CH264DecMediaObject`, and adds it under the name `H264 decoder`. Thus H.264 decoder intent is directly evidenced; it is not merely inferred from a file suffix. The wrapper class reference is at `0x103656D8`; its category argument is at `0x10365DA0`. Exact bytes for these debug GUIDs were unavailable and are not reconstructed from external assumptions.

However, the recovered graph has substantial defects:

1. `PalMemoryAlloc(0x1C, 1)` allocates seven pointer fields. `PalMemoryAlloc_0` at `0x10247F10` explicitly zeroes the allocation when bit 0 is set.
2. The H.264 loader assigns decoder, renderer, and converter fields at offsets `+4`, `+8`, `+16`, and `+20`. The separately constructed source filter is stored in video state `a1[12]`. The allocation's fields `+0` and `+12` remain zero. No call in this function receives their addresses to populate them.
3. Its first connection passes those zero fields as source and audio decoder to `sub_1011E392`, a thunk to `0x1024AB90`. This calls `0x1024ADA0` with the source; the recovered function dereferences that source before a null check.
4. Subsequent connections also expect the unset source field. No MP4 container splitter is constructed in this recovered branch.

This is strong static evidence of an incomplete graph. It is not an observed native crash, and no exact runtime HRESULT is established: earlier COM initialization can fail, and the apparent null dereference would precede the helper's normal `E_FAIL` paths. Successful MP4 playback is therefore unproved even for this debug build. Neither suffix recognition nor the `H264 decoder` label establishes a usable container-to-decoder pipeline.

## Reviewed export boundary

The release disassembly confirms the following ABI shape for `PalVideoPlay`:

```c
int32_t __cdecl PalVideoPlay(const char *filename, uint16_t playback_flags);
```

The filename is a required, caller-owned, NUL-terminated narrow resource path. It is read synchronously and copied into private engine state; the caller's pointer is not retained. The second argument occupies a four-byte Win32 cdecl stack slot, but only its low 16 bits are copied to state offset `+0x5A`. `uint16_t` expresses the confirmed bit width; signed interpretation and individual flag meanings remain unresolved. The upper bits must not become a separate portable behavior. Mode is a private WORD at `+0x58`, and the internal path buffer begins at `+0x5C`; this is not a caller-shared structure.

All three callees end in plain `RET`, return EAX 0 on suffix/initialization failure, and return EAX 1 after requesting playback. The Totsulover EXE caller at `0x45BA63` pushes the four-byte flag value, pushes the filename at `0x45BA6F`, and calls import `0x4EBC98` at `0x45BA70`. The `ADD ESP, 0x18` at `0x45BA76` cleans up four earlier `wsprintfA` arguments and the two PAL arguments together. This confirms calling convention and argument order independently of IDA's prototype. This contract is registered in `reverse/abi/pal32.json` and generated into `pal32.h` and the Rust native function-pointer declarations.

The alternative `PalMSpriteLoad` workers are audited separately; these release conclusions apply specifically to `PalVideoPlay` and its directly selected graph builders.

## Validation limits

Scope is `PalVideoPlay` and its directly selected graph builders. Alternative `PalMSpriteLoad` worker paths were not traced in this timebox; no assertion is made about every engine export or uninspected version. No native Windows playback was executed. No loose `.mp4`, `.m4v`, `.h264`, or `.264` sample was found under the Koikake fixture or Totsulover game roots; archive contents were not exhaustively searched for this audit. `testcase-cn` was absent. Renaming an MP4 file to another suffix was not tested and would not establish the original script's supported movie contract. The native release evidence supports retaining WMV/MPEG behavior and withholding any MP4/H.264 parity claim until a different working native implementation is found and verified.
