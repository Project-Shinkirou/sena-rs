# Console emulator execution and Vita memory audit

Date: 2026-10-24. No subagents or regression tests were used. Original game
resources remain read-only; emulator installs, resource symlinks, saves, replay
files and logs live under `target/reverse-audit/emulators`.

## Final checkpoint-based Vita runs

The final VPK SHA-256 is
`6a4db4984c801fd3f1c040868e882b448503cae206fc59ba1c5eeee21fe147d2`.
Both diagnostic snapshots were validated in a fresh desktop host, then restored
before the first native Vita frame. Normal host input advanced the VM from there;
no private game variable or forced script jump was used.

| Segment | Snapshot source frame / slot | Native frames after restore | Result |
| --- | --- | --- | --- |
| Later allocation interval | 2250 / 997 | 400 | Normal exit, no allocation failure |
| Large standing-art interval | 2170 / 996 | 240 | Normal exit, six 67,108,864-byte image allocations completed |

The image segment recorded a 25,518,080-byte maximum small-heap arena, a
258,863,104-byte large-block reservation high-water mark, 83,361,792-byte
CDRAM peak and 2,019,328-byte uncached scene peak. All stay within the respective
48/288/80/8 MiB budgets. The allocation segment's large-block high-water mark was
255,172,608 bytes. Large-block reservations, active/retired textures and tracked
GPU reservations were zero after both hosts were destroyed. Small-heap values
are sampled and must not be treated as exact allocation high-water marks.

Logs and replay/checkpoint/package hashes are retained in the
[machine-readable record](evidence/console-emulator-validation.json), under
`targeted_checkpoint_runs`. The 64 + 272 MiB predecessor failed its reservation
budget at frame 2313; the final profile rebalances the same combined 336 MiB
ceiling to 48 + 288 MiB. These finite restored segments do not establish a full
uninterrupted route, reproduce every prior cache/decoder position, or certify
real hardware. The earlier flat-frame prototype is not counted as image-pressure
acceptance.

[ADR-0012](adr/0012-host-diagnostic-checkpoints.md) records the diagnostic host
API and snapshot version 16. The reusable
[checkpoint creator](../platform/scripts/create_checkpoint.py) takes an isolated
game root, fixed-timestep input replay, source frame and save slot, writes the
snapshot and validates it in a fresh host. The Vita replay header accepts
`100,240,996` to restore slot 996 and run 240 frames at 100 ms per VM step.
Replay files were disabled after validation and the temporary default Vita3K app
symlink was removed. Original game resources were not modified.

## Reproduced failures and corrections

Switch initially stopped with `asset not found: archive.dat` even though a C
`stat` call found `data.pac`. The build log showed that the libc 0.2.186 layout
patch was unused; std linked registry libc 0.2.178. Matching and checking that
version restored actual boot. Ryujinx displayed the title and ADV text with the
native deko3d scene renderer. A preliminary 6,000-frame replay exited normally;
this was not a full game-route comparison.

Vita3K initially booted, but replaying into subsequent scenes hit a Rust
allocation failure around frame 1,128. The installed allocation hook recorded
`requested=104857600 alignment=4`. The original decode strategy grew a whole
track vector and then copied into an `Arc` allocation. The corrected strategy
counts frames first and writes a second decode directly into the final shared
allocation. Separate console lockfiles now use the desktop's pinned Kira 0.12.1.

The newer main-story replay exposed a separate missing four-argument byte
substring wrapper at `18:119`. Leaving its arguments on the stack restored a
dynamic-string handle as a work-bank base and eventually requested
2,147,485,784 bytes. Local IDA confirmed its actual registered handler, and
also confirmed that newer gosub resolves an operand rather than a literal
point ID. Both corrections and their signatures are retained in
[ADR-0008](adr/0008-preserve-newer-substring-stack-and-gosub-operands.md).

Later Vita standing-art loads still exceeded the practical memory scope of
the first correction. The revised pixel pipeline transfers decoder allocation
ownership to shared textures, streams GE/PGD3 prediction into final pixels, releases
expired FFI scene views before stepping, and compacts retired previous-scene
captures on Vita. [ADR-0009](adr/0009-retain-decoded-pixel-allocations.md) records
these representation changes separately from PAL geometry and pixel semantics.

C allocator wrappers subsequently confirmed that the newer replay could still
fail `calloc(67108864, 1)` with fragmented free space in the 336 MiB heap.
The exact report is preserved in
`target/reverse-audit/emulators/vita/fs/ux0/data/sena/newer-clocked-allocator-failed.log`.
This is a real allocation failure and is not counted as a passing run. Vita's
VM BGM loader now routes group-3 Ogg resources through bounded streaming, with
exact codec reset/preroll and the same native fade envelope; see
[ADR-0010](adr/0010-stream-vita-ogg-bgm.md). The resource-level and actual script
load paths are both connected. MIX/WMA/movie audio retain separate paths.

## Audio preservation check

A temporary comparison utility under `target/reverse-audit/audio-compare`
loaded the original read-only resources and compared every output sample's f32
bits against Kira's prior static decoder. It was not added as a regression test.

| Resource | Sample rate | Frames | PCM comparison | FNV-64 of sample words |
| --- | --- | --- | --- | --- |
| BGM01.OGG | 44,100 | 7,931,328 | Every sample equal | `8b37c0b82b3e9110` |
| BGM02.OGG | 44,100 | 6,612,928 | Every sample equal | `135e6d2b2ce8ed11` |

Log: `target/reverse-audit/emulator-audio-compare.log`.

Worker lifetime was checked separately: the upstream streaming sound retained
one traced decoder after manager destruction; the locally retained Kira adds a
sound destructor that marks its scheduler stopped, and the same diagnostic then
reported zero. Logs: `target/reverse-audit/emulator-stream-teardown-before.log`
and `target/reverse-audit/emulator-stream-teardown-after.log`. The mixer comparison
was repeated after that fix, with unchanged output. See
[dependency provenance and licenses](../vendor/kira/UPSTREAM.md).

The bounded stream decoder also matched every f32 sample bit for both tracks
and six 2,048-frame seek windows per track (including backwards seeks and the
tail). Its largest decoded packet was 1,024 frames. A separate diagnostic
compared 256,000 mixed stereo frames through the same PAL amplitude wrapper:
start at sample 22,050, loop 1–2 seconds, rate 1.1, pan 0.2, volume -6 dB,
200 ms fade-in, 100 ms pause/resume and 150 ms stop fade. Every i16 output
sample matched the static path (`90d53a627fa0e250`). These are manual diagnostic
programs, not new regression tests. Logs:
`target/reverse-audit/emulator-stream-audio-compare.log` and
`target/reverse-audit/emulator-stream-mixer-compare.log`.

## Image preservation check

A separate temporary utility compared complete dimensions, cell geometry,
offsets and every RGBA byte with the pre-change decoder. All seven resources
matched, and the resulting scene texture retained the decoder Vec's original
pixel pointer. Log: `target/reverse-audit/emulator-image-compare-streaming.log`.

| Resource | Dimensions | Pixel bytes | RGBA FNV-64 |
| --- | --- | --- | --- |
| ST33A_E010.pgd | 4096x4096 | 67,108,864 | `041ea873b4170a10` |
| ST33A_E040.pgd | 4096x4096 | 67,108,864 | `81cecdcfdaa94312` |
| ST03A_A010.pgd | 2048x2048 | 16,777,216 | `42f818ca88fb3dd5` |
| BKL102A.JPG | 2640x1486 | 15,692,160 | `e44004a5cb2c5ff7` |
| ST07A_A040.PGD | 1106x2048 | 9,060,352 | `b02fd7c86d07b183` |
| BK000D.PGD | 1680x928, 24-bit GE input | 6,236,160 | `6c163507cf6047d9` |
| BK000DC.PGD | 1680x928, 24-bit GE input | 6,236,160 | `47323619bfadef88` |

## Memory measurement

The Vita host records newlib heap arena/used/free figures, kernel free-memory
queries, active/retired texture byte counts, and independently tracked mapped
GPU allocations and peaks. The final 80 MiB CDRAM and 8 MiB uncached budgets include
retired textures. A finite CSV input replay supplies host mouse/skip events.
With a fixed 100 ms timestep, the replay now drains the full matching PCM frame
count on the VM clock rather than consuming audio at wall-clock speed. This
executes decoding/mixing but discards output; it is not an audible playback
check. A zero timestep keeps the real host clock and native AudioOut worker.
Earlier fixed-timestep/real-worker runs are preserved with that distinct scope.
Physical controller/touch input remains a separate validation scope.

The installed Vita3K version has a counter defect described in
[ADR-0007](adr/0007-verify-console-runtime-and-memory.md). Its reported CDRAM
free size reached zero and subsequently values larger than the physical pool.
Those raw figures must not be interpreted as a measured leak or as available
hardware capacity. The application retains bounded allocation accounting.

Final run hashes, results, peaks and limitations are retained in
[the machine-readable validation record](evidence/console-emulator-validation.json).

The initial budgeted Koikake replay completed 2,400 frames on both consoles.
It used a 320 MiB CPU heap and 16 MiB uncached budget. The Vita run
sampled the heap every 120 frames; its sampled maximum used heap was
127,785,064 bytes (121.87 MiB), and maximum heap arena was 245,211,136 bytes
(233.85 MiB). These are sampled values, not allocator high-water marks.
Per-allocation GPU accounting peaked at 79,691,776 bytes (76 MiB) in CDRAM and
9,396,224 bytes (8.96 MiB) in uncached RAM. After teardown, active and retired
texture counts and both tracked GPU allocation totals were zero. The remaining
reported heap use was 4,480 bytes. Switch also logged a normal frame-limit exit.

## Final package execution

Switch's final NRO completed both isolated replays normally: Koikake at 2,400
frames (last sampled PC `0x7C278`) and the newer main story at 3,600 frames
(last sampled PC `0x15AA00`). The latter retained 228,231,232 bytes of referenced
source pixels in its final sampled scene; that figure is not unique allocation
accounting and is not a Vita heap measurement. Final NRO SHA-256:
`c71168dfa267833f3e06032dadde2bded8002c89e02c66abd639fbeb8b208527`.

The BGM-streaming single-arena profile later failed at frame 2,379 despite
88,539,032 bytes of free heap. That run remains a failure; its log is
`target/reverse-audit/emulators/vita/fs/ux0/data/sena/newer-stream-fragmentation-failed.log`.
[ADR-0011](adr/0011-separate-vita-large-allocations.md) records the resulting
allocation policy: 48 MiB fixed newlib plus at most 288 MiB of separate large
cached memory blocks, including headers and page rounding. The combined CPU
ceiling remains 336 MiB. Large-block live and peak reservations are recorded
separately; a lower newlib used/arena figure alone does not mean lower total use. Scene allocations separately obey 80 MiB CDRAM / 8 MiB uncached
budgets, including deferred retirement. Code, stacks, vita2d and other runtime
allocations require additional headroom outside those scene budgets.

## Acceptance boundaries

Actual native packages were executed in local emulators, rather than replacing
these runs with desktop FFI checks. Visual observations include title artwork,
ADV text and scene changes. PAL raster frames can still use the
documented Vita CPU fallback. Simulator runs do not establish real-device memory
fragmentation, sustained frame rate, audible output, mobile lifecycle behavior,
or complete engine compatibility. Accelerated skipping can expose pre-existing
VM behavior and is not interchangeable with normal real-time play.
