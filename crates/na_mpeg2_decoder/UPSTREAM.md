# Upstream

Imported from https://github.com/xmoezzz/siglus_rs at commit
`5f358ef1360b0439e92aed2b749e8f25d8721954`, `crates/na_mpeg2_decoder`.
The upstream repository is licensed under MPL-2.0; a copy is retained here.

This import contains the video, demux, conversion and video-pipeline
sources. The standalone players and the separate audio pipelines are omitted.
The manifest and lib.rs expose that subset without desktop or audio dependencies.
Six upstream let chains were rewritten as equivalent Option filters/equality
checks to retain this workspace's Rust 2021 / Rust 1.87 compatibility.
