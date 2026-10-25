# Kira provenance and local change

Upstream: [tesselode/kira](https://github.com/tesselode/kira), crate 0.12.1.
The crates.io package's VCS record identifies commit
`88807ba6f9488e6ae5d305b53e4640e303af376f`, path `crates/kira`.
Registry package SHA-256: `065632c16b39f4c00bc5514de0698474d1b6c36120743a154c78ca27ffc1ffe1`.

License: MIT OR Apache-2.0. Both upstream license texts are included here.
Source, metadata and original upstream tests were copied from the locally cached
0.12.1 registry package. No upstream tests were authored or run for this change.

The sole source modification adds `Drop for StreamingSound` in
`src/sound/streaming/sound.rs`. It publishes `PlaybackState::Stopped` to the
shared scheduler state when the sound is destroyed, including when its mixer
or an outer PAL sound wrapper is destroyed without another audio callback.
The existing decoder loop then exits rather than keeping the decoder, encoded
bytes and worker alive while waiting on a full abandoned frame queue.
No sample conversion, interpolation, loop, gain or normal playback code changed.

A temporary traced-decoder program reproduced one live worker after manager
teardown with upstream, and zero with this change. Results and console execution
scope are retained in [ADR-0010](../../reverse/adr/0010-stream-vita-ogg-bgm.md)
and the [console report](../../reverse/console-emulator-validation.md).
