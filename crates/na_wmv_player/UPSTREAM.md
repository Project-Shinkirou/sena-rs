# Decoder provenance

Library source synchronized from [xmoezzz/siglus_rs](https://github.com/xmoezzz/siglus_rs/tree/5f358ef1360b0439e92aed2b749e8f25d8721954/crates/na_wmv_player), commit `5f358ef1360b0439e92aed2b749e8f25d8721954` (2026-10-23 import). The source repository distributes this code under MPL-2.0; its license is included alongside this file. Desktop player binaries and optional desktop dependency changes were not imported.

The ASF API retains every pending payload across `next_frame` returns for both video and audio. This is the upstream equivalent of sena_rs's multiple-payload regression fix.

The imported decoder supports WMV3 Simple/Main and progressive WVC1 paths, plus WMA Professional. Advanced interlaced pictures, range mapping, and slice start codes remain explicit errors; they must not be advertised as supported. Existing WMV2 and WMA synthetic fixtures are exercised by `pal-vm/tests/movie_timing.rs`.
