# SoftPAL MPEG decoder integration

## Evidence and source

This is a codec implementation integration, not proof that every original PAL
movie command has been recovered. Source:
https://github.com/xmoezzz/siglus_rs, commit
`5f358ef1360b0439e92aed2b749e8f25d8721954`,
`crates/na_mpeg2_decoder`. Its MPL-2.0 license is retained in the imported crate.
No AGENTS.md exists in that upstream revision. Only video, demux, conversion and
video pipeline modules were imported; audio and desktop player modules were not.
Six let chains have equivalent Rust 2021 rewrites to retain the repository's
declared Rust 1.87 compatibility. Tests were actually run with Rust 1.95, not 1.87.

## Pipeline and PAL integration

`pal-vm/src/msprite/mpeg.rs` feeds compressed bytes to the Rust pipeline in 16 KiB
chunks. The demuxer recognizes MPEG program streams and separates PES video from
other stream types. The decoder handles MPEG-1/2 I/P/B pictures and holds delayed
reference pictures until presentation order permits output; end-of-stream flush
releases the final delayed reference. Decoded frames are converted from YUV to
RGBA using the upstream BT.601 limited-range conversion. No FFmpeg subprocess or
native codec library is used by runtime playback or tests.

MSprite accepts MPEG program-stream pack magic and MPEG elementary sequence-header
magic. The upstream demuxer also contains transport-stream code, but MSprite does
not advertise or route TS input in this change.

The adapter subtracts the first displayed PTS origin, accounts for the MPEG
33-bit wrap, and ignores backwards anchors. PES timestamps are coarse: several
pictures may share one PES PTS. Missing or repeated anchors advance by the exact
sequence frame-rate rational. A 360 kHz internal cadence represents all eight
base frame-rate codes without rounding accumulation; MPEG-2 sequence extensions'
frame-rate numerator/denominator are included. Only presentation milliseconds are
rounded down. This prevents a PS muxer's initial 540 ms timestamp offset from
appearing as an artificial startup delay.

The MSprite presentation clock, future-frame queue, pause/lock behavior and loop
remainder logic are retained. A rewind resets decoder reference state and compressed
input position. Nonzero loop points preroll through the normal presentation clock,
keeping the frame preceding the requested time until the following frame is due.
This is correct but linear-time seeking, not an indexed GOP seek.

## Validation

`cargo test -p pal-vm --test movie_timing --test mpeg2_movie`: 10 passed.
The one-picture MPEG/WMV regression fixtures retain their 40 ms duration instead
of finishing after the old 1 ms unobserved-frame-interval fallback. At EOF the
MPEG adapter uses its sequence-rate rational for the final picture; WMV uses the
ASF duration when valid. An eight-picture 24000/1001 fps MPEG-2 fixture verifies
the final picture lasts through 332 ms and finishes at 333 ms (the preceding
observed interval was only 41 ms). Single-picture looping retains large-delta
remainders. These are adapter fixes, not additional native command discoveries.
The existing MPEG-1/WMV tests check 25 fps presentation against 16/39/1 ms update
steps, final-frame duration, whole-stream loops and non-frame-aligned loop points.
The additional MPEG-2 fixture has ten 32x32 frames at 25 fps, including six B
pictures (independently confirmed using ffprobe). Both the complete ten-frame
presentation and a 50..150 ms loop pass. FFmpeg is needed only to regenerate the
original test pattern, not to run these tests:

```
ffmpeg -f lavfi -i 'testsrc2=size=32x32:rate=25:duration=0.4' -an -c:v mpeg2video -g 10 -bf 2 -f mpeg timing_mpeg2_bframes.mpg
ffmpeg -f lavfi -i 'testsrc2=size=32x32:rate=25' -frames:v 1 -c:v mpeg2video -f mpeg timing_mpeg2_single.mpg
ffmpeg -f lavfi -i 'testsrc2=size=32x32:rate=25' -frames:v 1 -c:v wmv2 -f asf timing_wmv_single.wmv
ffmpeg -f lavfi -i 'testsrc2=size=32x32:rate=24000/1001' -frames:v 8 -c:v mpeg2video -g 8 -bf 2 -f mpeg timing_mpeg2_23976.mpg
```

## Remaining boundaries

- MPEG soundtrack output is not added by this video integration.
- MP4/H.264 remains unsupported by the existing dispatcher.
- Interlaced/field-repeat timing, variable sequence-rate changes, damaged streams,
  and game-scale MPEG-2 pixel conformance have not been validated here.
- The imported decoder itself is not independently proven bit-exact against a
  reference decoder. Passing presentation tests does not establish codec conformance.
- Seeking decodes from the beginning and may be slow for long movies.
- Compressed source bytes are retained in memory, as with the previous decoder;
  decoded frame buffering is incremental rather than whole-movie RGBA allocation.
