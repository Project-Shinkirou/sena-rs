# WMV / WMA decoder synchronization, 2026-10-23

## Provenance and scope

The user authorized reuse of `siglus_rs`. Library sources in `crates/na_wmv_player/src` were synchronized from [xmoezzz/siglus_rs commit 5f358ef1360b0439e92aed2b749e8f25d8721954](https://github.com/xmoezzz/siglus_rs/tree/5f358ef1360b0439e92aed2b749e8f25d8721954/crates/na_wmv_player). MPL-2.0 is retained as `crates/na_wmv_player/LICENSE-MPL-2.0`; `Cargo.toml` declares that license. No source AGENTS.md was present. This is codec-source reuse, not new proof that the SoftPAL engine implements every codec path.

Desktop binaries and their dependency changes were not imported. There are no additional dependencies. Six Rust 2024 let-chain conditions were rewritten equivalently for this crate's Rust 2021 edition and workspace MSRV 1.87. The old two-argument `Wmv3Decoder::decode_frame_owned(payload, pts_ms)` remains available; ASF uses the new explicitly named method with its key-frame hint.

## Stream semantics

`AsfFile::read_packet` returns ASF payload fragments. Several complete media objects can occur in one packet. `FrameAssembler` joins fragments by stream/object identity and covered byte ranges; a decoded picture or PCM block is returned only after the complete object is available. Both `AsfWmv2Decoder` and `AsfWmaDecoder` retain unread packet payloads in a `VecDeque` across `next_frame` calls. Returning the first frame must not discard subsequent objects: doing so drops video frames and desynchronizes the WMA bit reservoir. This preserves the local multiple-payload fix using the upstream equivalent.

Video selection recognizes WMV1, WMV2, WMV3 and WVC1. WMV1/2 retain the existing reconstruction path. WMV3 now parses Simple/Main picture headers and reconstructed macroblocks with the upstream reference-frame, motion, transform, prediction, overlap and filtering code. Its internal reference surface is rounded to 16-pixel macroblock bounds, then cropped for the displayed image; discarding non-visible rows (for example 1080..1087) corrupts later prediction. WVC1 adds progressive Advanced Profile sequence/entry-point headers, emulation-prevention unescaping, and picture headers on top of the shared reconstruction code. ASF PTS remains the playback clock.

Audio selects tags 0x0160 (WMA1), 0x0161 (WMA2), or 0x0162 (WMA Pro). The import includes exponent validation, IMDCT/table fixes, and WMA Pro packet-reservoir, variable-block and channel-transform handling. EOF flushes buffered output once. Errors retain codec tag, block alignment, object length, and PTS context. Color conversion helpers include BT.601 and BT.709; importing a helper alone does not change PAL's caller-side conversion.

## Verification

- `cargo test -p wmv-decoder --lib`: 15 passed (VC-1 header/bitplane syntax, color conversion, WMA exponent and IMDCT tables, WMA Pro VLC tables).
- `cargo check -p wmv-decoder --no-default-features --features video-only`: passed.
- `cargo test -p pal-vm --test movie_timing`: 5 passed, including all five 25fps synthetic WMV frames, frame timing/loop remainder, and WMA media-object timestamps `[0, 46, 92, 139, 185]`. MPEG checks in this shared regression also passed.
- `git diff --check -- crates/na_wmv_player`: passed.

The imported WVC1 syntax fixture had an incorrect expected sequence rate of 60000/1001. Its bytes encode `FRAMERATENR=3, FRAMERATEDR=2`, hence 30000/1001 under the [FFmpeg sequence parser](https://github.com/FFmpeg/FFmpeg/blob/master/libavcodec/vc1.c). The assertion now checks this encoded rate. FFprobe reports a 60000/1001 timing rate on the same short pulldown stream; that container/parser timing convention must not be confused with the sequence field. The partial probe cannot verify picture reconstruction.

## Remaining limits

This run does not establish pixel-exact WMV3/WVC1 reconstruction or listening accuracy for WMA Pro on complete game movies. WVC1 interlacing, range mapping, slice start codes, pan-scan picture syntax, and progressive segmented frames are explicitly rejected. Some WMA Pro layouts/transforms also return Unsupported. WMV2 still contains the upstream documented macroblock-level motion-predictor approximation. MP4/H.264/AAC is outside this ASF decoder and was not added. Original SoftPAL movie audio scheduling and unsupported codec behavior need separate engine evidence.
