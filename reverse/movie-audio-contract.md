# Movie and sound contracts

This document records native behavior recovered from copied IDA databases on 2026-10-23. The primary build is Totsulover `Pal.dll`, SHA-256 `744219b7c2b2ba80c5bb0440ac556201652ffbcc01d2de1211c4b576ad21db7e`. Comparisons use `reverse/PAL.dll` (`e676972ddaab2d45fc82210bb89143923f10d1053492a89e50571dbcec54840a`) and Koikake `testcase/dll/Pal.dll` (`2a43bc4408aff086c18dc73ccd2d35be4f8a56cf8b3e82da7b75f80ffe61625e`). The reviewed export inventory is `contracts/audio_video.json`. Status fields distinguish ABI verification, behavior verification, and portable implementation mapping.

## Movie sprites carry audio

Totsulover `PalMSpriteLoad` (`0x100AED90`) builds its worker through `0x100AE920` and `0x100AE420`. It creates an archive-aware source, MPEG stream splitter, MPEG video decoder, and custom `CDShowRenderer`. When the splitter exposes an audio pin, it additionally connects an MPEG audio decoder and DirectSound renderer, obtains `IBasicAudio`, and applies a volume. Video and audio belong to the same DirectShow graph and its run/stop/seek operations. Keeping movie sprites permanently silent is incompatible with this implementation.

The older release loader `0x100AC9C0` and Koikake loader `0x100BA8D0` likewise construct audio decoder/output branches. The video texture renderer is not an audio clock or an audio output device. Portable playback needs a PCM output path synchronized to movie presentation, including seeks and completion, rather than only decoded frame uploads. No Windows listening or device-timing comparison was performed in this audit.

Evidence: `evidence/msprite-{totsulover,reference,koikake}-load.json`, `evidence/msprite-{reference,koikake}-audio.json`.

## Native lifecycle and loop boundaries

The worker's private fields in all three inspected releases include:

| Offset | Meaning |
| --- | --- |
| `+0x04` | `IMediaControl` |
| `+0x08` | `IMediaEventEx` |
| `+0x0C` | `IMediaSeeking` |
| `+0x10` | Optional `IBasicAudio` |
| `+0x24` | Custom texture renderer |
| `+0x34`, `+0x38` | Signed 32-bit loop start/end in milliseconds |
| `+0x48` | Stored lock flag |
| `+0x4C` | Raw loop argument, tested for nonzero |
| `+0x50` | Raw state word |

These offsets are not a caller-shared ABI struct. `PalMSpriteCheck` returns the worker pointer from sprite offset `+0x14`, rather than normalized 1. The VM wrappers use it only as a nonzero check. `PalMSpriteInfoCreate` temporarily builds a worker to obtain dimensions, destroys it, and clears the pointer; an information-only object is not a playable movie sprite.

`PalMSpritePause` is a no-op for a non-null sprite in all three releases. `PalMSpriteLock` and `Unlock` only store the worker flag and renderer flag at `+0x188`; they do not invoke graph pause, stop, or seeking. In the newer release, the sample renderer `0x100C3540` keeps copying samples and never reads that lock flag. Its scheduling hook `0x100C3530` returns 1. An instruction operand scan found only initialization and writes to this renderer field; remaining `+0x188` references are unrelated COM-vtable calls. Freezing the whole movie clock or its audio when locked is unsupported by this evidence. See `evidence/movie-audio-lock-field-refs.json` and `evidence/msprite-totsulover-render-samples.json`.

The raw state starts at 0. A successful play sets `(old & 0x80000000) | 1`; a stop of a running graph sets `(old & 0x80000000) | 2`; natural nonlooping completion sets `(old & 0x80000000) | 6`. The finished bit `4` therefore distinguishes natural completion from manual stop. `PalMSpriteIsLoop` returns only `state & 0x80000000`, even though `SetLoop` and `Play` write the separate `+0x4C` integer. Do not synthesize state bit 31 from that integer or from nonzero loop points. No setter of state bit 31 was identified in the reviewed control paths.

`SetLoopPoint(sprite, start_ms, end_ms)` always stores the signed start, stores the end only when it is nonzero, and performs no seek. The initial end is graph duration divided by 10,000. Neither setter clamps negative values. `Play` requests graph Run without seeking to the configured loop start. Therefore the first pass normally runs from the graph's initial position to its initial stop position. Only when an `EC_COMPLETE` event is processed and the loop integer is nonzero does the render callback pause, call `SetPositions(start_ms * 10000, ABSOLUTE, end_ms * 10000, ABSOLUTE)`, and run again. Applying the configured loop end to truncate the first pass changes native behavior.

The newer `Play` helper bounds a failed Run/state-wait to about one second, then the public export stops and retries. Older builds wait for graph running without that timeout. Stop waits up to approximately three seconds for the graph to leave running state. Public `Stop` returns 1 for any non-null sprite regardless of helper result. Completion polling occurs in the enabled sprite render callback, so hidden rendering and completion delivery require care when comparing timing.

Evidence: `evidence/msprite-totsulover-controls.json`, `evidence/msprite-totsulover-worker.json`, `evidence/msprite-{reference,koikake}-worker.json`.

## Movie gain and script contracts

Let `M` be the native master level at context `+0x4940`, and `G0..G6` the seven group levels starting at `+0x4944`. Normal levels use 0..10000. The DirectSound/DirectShow conversion truncates integer products first, converts to single-precision float, and uses `trunc(2000 * log10(q / 10000.0))` hundredths of a decibel. Zero maps to `-10000`. Native setters do not all clamp invalid values, so portable validation must not be mistaken for a recovered native clamp.

- Initial movie-sprite level: `q = trunc(G4 * M / 10000)`.
- `PalMSpriteSetVolume(sprite, percent)`: `q = trunc(M * trunc(percent * G4 / 100) / 10000)`.
- `PalMSpriteSetVolumeEx(sprite, percent)`: `q = trunc(100 * percent * M / 10000)`. This bypasses group 4.
- Both sprite setters return 0 without a movie worker/audio interface and 1 after requesting the volume; they do not propagate the COM setter's HRESULT.
- `PalSoundSetVolume` updates DirectSound channel arrays, not already-created movie-sprite `IBasicAudio` interfaces. There is no observed automatic replay of the sprite-specific percentage when a group/master setting changes.

Full-screen video selects a level in `0x100C08A0` using the low two bits set by `PalSetVideoVolumeType`: mode 0 uses `G3`, mode 1 `G0`, mode 2 `G0 * G3 / 10000`, mode 3 `G3 - G0`. It clamps that result to 0..10000 before applying master conversion. `PalSetVideoVolumeType` returns the changed low-bit mask, not the selected value. `PalVideoSaveState`/`RestoreState` copy three private global words; they are not decoder-position save/restore APIs.

The newer EXE registers these wrappers in category 11:

| Index | Arguments in pop order | Behavior |
| --- | --- | --- |
| 0 | filename, flags | Full-screen movie request; low 16 flag bits reach PAL |
| 1 | filename, slot, x, y, z, loop mode, start ms, end ms | Reuses generic sprite setup, then sets loop points and plays |
| 2 | slot | Delegates one pop to generic sprite clear; -1 clears all 128 generic slots |
| 3 | sprite slot | Waits for state bit 4, first clears loop mode |
| 4 / 5 | sprite slot | Lock / unlock flag calls |
| 6 | sprite slot, loop integer | Starts movie sprite; preserves wrapper replay metadata |
| 7 | sprite slot | Stops movie sprite |
| 8 | volume selector | Calls `PalSetVideoVolumeType`; no destination store |
| 9 | percent | Stores `100 * percent`; applies group 0 unless muted |
| 10 | none | Returns stored movie volume divided by 100 |
| 11 | mute word | Nonzero means muted; applies zero or stored level to group 0 |
| 12 | sprite slot | Returns raw movie state, 0 for an existing nonmovie image, -1 for invalid slot or no image |
| 13 | sprite slot, percent | Clamps percent 0..100 and calls `PalMSpriteSetVolumeEx`; returns 1 for a movie, 0 for an existing nonmovie, -1 for invalid slot/no image |

Index 13 returns 1 after a successful movie type check regardless of the PAL volume call's result. Slots are unsigned-checked against 127. Category coordinates come from the registration table, not names inferred by the decompiler. Evidence: `evidence/totsulover-command-map.json`, `evidence/movie-audio-totsulover-wrappers{,-extra}.json`, and `evidence/movie-audio-totsulover-volume.json`.

Old reference `Game.exe` wrappers at `0x42E650` through `0x42DFA0` have the same pop order for indexes 0..7. Index 0's second word is a cancellation-input flag, not a target layer. Index 2 at `0x42E350` delegates through `0x4BB04C` to `0x428E20`, which consumes the slot; counting only the thin wrapper incorrectly reports zero arguments. Newer index 2 likewise delegates through `0x4EAE44` to `0x44C090`. Index 3 consumes one argument only on its first entry and none on subsequent polls; counting duplicated stack-underflow branches does not justify arity 2. It returns without stopping the movie when state bit 4 or a nonzero scheduler state releases the wait. All indexes 0/2/3/4/5/6/7 return control 1 without writing a VM destination. Index 1 differs: the old wrapper has no destination store, while the newer wrapper writes the PAL play result and returns control 0 on failure. Old generic clear accepts the boundary slot 128 in its machine comparison, whereas the newer comparison stops at 127; that old out-of-bank edge is not a reason to make unchecked portable accesses.

Full-screen cancellation is processed by newer `0x100C0F60`. Nonzero playback flags select input mask `0x01800007`; zero selects `0x01000001`, with Alt+Enter excluded. When the native fade gate and custom renderer are available, cancellation enters phase 2 and linearly reduces both audio level and renderer alpha over the default 1000 milliseconds. It recomputes the selected video group/master gain during this fade, then sends event 2, clears input, and waits 100 ms. Without that gate it aborts immediately. The three words saved by the decorated Save/Restore exports are fade phase, start clock, and duration. Display-device recreation separately destroys the graph, rebuilds it, seeks to remembered 100-nanosecond positions, and resumes. Evidence: `evidence/movie-audio-video-{clock,lifecycle,state-refs}.json`.

## Full-screen video is an asynchronous global operation

`PalVideoPlay` synchronously constructs its graph but returns after requesting
Run. The newer export at `0x100C1200`, reference `0x100BACB0` and Koikake
`0x100C8A50` all lower render-thread priority, call `IMediaControl::Run`, set
PAL context `+0x14` to 1, post window messages and return 1. The Run HRESULT is
not tested there. A nonzero graph-initializer result returns 0 before that start
sequence. There is no synchronous EOF loop and no script sprite-slot argument.
This movie occupies private PAL global state; its cleanup never deletes or
replaces an ordinary script sprite slot.

The newer initializer `0x100C0910` applies the initial group-selector/master
volume through `IBasicAudio`, configures event notification `0x8064`, and sets
phase 1 with a 1000 ms duration. Phase 1 is ordinary running state, not a
fade-in. Input thread `0x100A2780` continues sampling and calls cancellation
helper `0x100C0F60` while `IsMovie` is nonzero. It continues traversing PAL tasks:
`0x100BE460 -> 0x100BDED0` has no movie-state gate. The game layer applies the
gate: newer EXE frame callback `0x476F80` and Koikake `0x432CC0` skip their normal
scene/script update body while `IsMovie` is nonzero. Some window-mode callback
handling precedes that newer gate. Globally freezing every host task is not the
same operation.

Zero playback flags permit Escape/right-mouse cancellation (`0x01000001`). A
nonzero low word adds Space/Return/left mouse (`0x01800007`). These masks test
push edges, and physical Alt+Enter is excluded. Wheel and arbitrary held-key
states are not cancellation triggers. The optional fade uses
`100 - trunc(float32(elapsed) / duration * 100)` percent while elapsed is below
duration. The abort event does not itself immediately clear `IsMovie`.

Window message `0x8064` drains and frees media-event parameters. Event 1
(complete) or 2 (user abort) invokes cleanup `0x100C0C40(movie,0)`, stopping and
releasing the graph, restoring cursor/thread priority and clearing `IsMovie`.
Event 3 takes a different native branch: cleanup with state 1 and an attempt to
recreate the graph. Device resets likewise preserve state 1 while rebuilding
and seeking remembered 100 ns positions. A portable clean termination on decoder
error is a deliberate safety policy, not proof that the native error branch is
equivalent.

Evidence: `evidence/movie-audio-totsulover-{async-lifecycle,task-gate,frame-gate}.json`,
`evidence/movie-audio-video-{clock,lifecycle}.json`, and the release export bodies
linked from the inventory. The detailed private helper lifecycle above is
scoped to the newer PAL; public asynchronous start is confirmed in all three.

Koikake category 11 is table `0x478980`, selected by category table
`0x478900[11]`. Its handlers 0..7 are `0x421BB0`, `0x421A20`, `0x4219E0`,
`0x421890`, `0x4217F0`, `0x421750`, `0x421670`, `0x4215D0`. All eight preserve
the VM destination. Set consumes eight arguments and returns control 0 if the
sprite is missing; otherwise control 1. Clear delegates to `0x4193C0`, which
consumes one slot and supports -1 for all 128 generic slots. Wait consumes one
argument on entry and none while polling; its rewind helper `0x4361D0` checks
only scheduler bit `0x10`, whereas the newer/reference wrappers check `0x30`.
Its completion conditions still include state bit 4 or any nonzero scheduler
state, and it does not stop the movie on completion. See
`evidence/movie-audio-koikake-{wrappers,delegation,sprite-delegates}.json`.

## Sound allocation, ranges, and asynchronous state

The newer sound banks contain 140-byte private records. Groups 0..6 have capacities `[2, 16, 8, 2, 16, 64, 16]`; their handle tags are `[0x10000000, 0x30000000, 0x70000000, 0x20000000, 0x40000000, 0x50000000, 0x60000000]`. `PalSoundGetChannelCount` counts free records, requiring both loaded and reserved fields to be zero. It does not return capacity or the number playing. Several legacy helpers default unsupported group values to another bank instead of rejecting them; record indexing is often unchecked. Native handles must remain 32-bit words at adapters.

`PalSoundLoad` and `PalSoundLoadEx` argument 3 is an optional borrowed pointer to four 32-bit words: enabled, initial start byte offset, loop start byte offset, loop end byte offset. Both WAV and Ogg loaders read this descriptor for group 3. A zero end selects decoded byte length. WAV seeks add the data offset before calling `SetFilePointer`; Ogg byte offsets are shifted right by two before `ov_pcm_seek`, exposing the native stereo-16-bit assumption. `PalSoundSetStartEnd` stores raw byte offsets, not movie milliseconds. The ABI declaration must describe this pointer/layout rather than call it a scalar flag. These are native API units; EXE wrappers can convert script values separately.

`PalSoundPause(handle, pause, fade_ms)` is distinct from the no-op movie-sprite Pause. It stores a pause latch, queues command 9/10, and optionally fades before the worker stops/resumes the DirectSound buffer. Fade state 1 is fade-in and 2 fade-out. `PalSoundGetState` returns this fade state, not the playing flag. The worker computes linear amplitude over elapsed engine-clock milliseconds, then converts amplitude to decibels. `GetStatus` instead returns the queued command word for one of the two group-3 records. `PlayIs` returns group-3's playing word or queries the DirectSound buffer for other groups.

Group-3 Stop closes its source and clears loaded state in the worker; callers should not assume every native stop preserves a reusable loaded track. Other ordinary sound groups retain their loaded buffer. Play, fade, stop, and release communicate with the worker and may wait for acknowledgements. A portable synchronous implementation may reproduce observable state while retaining its own synchronization, but queue/timeout behavior is not interchangeable with boolean state stubs.

`PalSoundLoadEx` is restricted to group 4 and applies its float gain to decoded signed 16-bit samples with truncation and saturation. `PalSoundSetVolumeRatio` only stores the ratio; it does not immediately rescale the existing PCM buffer. The newer voice-speed getter/setter access one float global, clamp writes to 0..200, and return the stored/previous value. The reviewed IDA references show no playback consumer of that global, so altering playback speed from this setting is not established native behavior.

Evidence: `evidence/audio-totsulover-{exports,helpers,streaming,state-machine}.json`, `evidence/audio-{reference,koikake}-exports.json`. The state-machine decompiler reports a stack-analysis warning for the worker thread; function-level contracts rely on the accompanying instructions and narrowly identified fields, not its guessed thread prototype.

## Portable transport integration

The movie transport now owns decoded WMV/WMA and MPEG/MP2 PCM separately from
ordinary PAL sound slots. Each movie owns its output handle; stop, release,
clear and destruction stop that handle. Start/resume uses the video's current
position, and a loop generation restarts audio at the same normalized position
as the video seek. Lock and the native no-op Pause do not freeze either stream.
The initial pass has no audio loop region, so stored loop bounds only apply
after the initial full-length pass. Movie gain is captured at load or an explicit
volume call; ordinary group/master updates do not silently change it.

Full-screen `movie_play` owns a private render surface and stores the second
operand's low 16 bits as cancellation flags. It does not replace a script sprite
slot. The VM yields when the movie starts; normal button/text/action callbacks
and VM work are gated during playback while generic PAL tasks continue. Natural
completion releases the private surface and soundtrack. Cancel masks, the
Alt+Enter exclusion, integer-percent fade and movie-only input clear follow the
reviewed native path. Newer and reference initialization enable the fade latch:
`pal-platform/totsulover-helpers.json` `0x100A34F0` stores context[283]=1;
`pal-platform/reference-helpers.json` `0x100A2C20` stores context[277]=1.

The older generic sprite-loading path was also confirmed during integration.
Koikake `0x434E30` uses table `0x478D3C`, initialized by `0x401720`, with
`.pgd`, `.mpg`, `.tga`, `.bmp`. Reference `0x447FE0` uses table `0x4BB670`,
initialized by `0x401090`, and adds `.png`, `.jpg`. Both append each suffix to
the supplied basename and use `PalMSpriteLoad` for `.mpg`; the newer six-entry
path is equivalent. This now permits ordinary older `sp_set` and delegated
`msp_set_loop_sp_ep` to create MPEG movie sprites. Evidence is preserved in
`evidence/movie-audio-{reference,koikake}-suffix*.json`.

Reviewed wrapper definitions are generated into the newer dispatch registry
from `contracts/audio_video.json`. The legacy catalog now uses filename/flags
and filename/slot/x/y/z/loop/start_ms/end_ms argument order. Clear consumes one
slot; Wait does not mistake manual-stop state2 for natural-finish state6.
Legacy movie controls preserve their VM destination. Newer state/volume queries
and the newer load result retain their distinct destination stores.

Validation covers synthetic real MPEG and WMV video timing, decoded soundtrack
output through the host PCM sink, lifecycle cleanup, first-pass looping, stored
volume semantics, isolated full-screen rendering ownership, cancellation and
VM pause/resume. Remaining boundaries are explicit: native DirectShow error-event
recreation/device reset is not reproduced; PCM decoding is bounded to256MiB
and retained in memory; the portable video clock uses engine elapsed time rather
than a measured hardware audio clock. No hardware audio/video synchronization
or Windows-native frame comparison is claimed by these tests.

## Ordinary sound transport corrections

`audio/native_sound.rs` now supplies an amplitude envelope around the portable
mixer. Native fades ramp amplitude before converting it to decibels; the mixer's
built-in decibel interpolation did not match this behavior. Pause preserves the
play position. Worker `0x100B07C0` prioritizes fade processing over command 10:
with a nonzero resume duration, a paused buffer remains stopped during the ramp
and resumes after it. Repeated Play stops the prior mixer object before starting
another, and dropping a sound handle stops its output. Group-3 Stop releases its
loaded source after playback has started; ordinary loaded buffers remain usable.
Production envelopes now sample the same wrapping engine millisecond counter
published by `AudioSystem::set_pal_time`, through 32-bit atomics suitable for
Vita. Audio playback still advances on its own sample clock. This prevents a
host audio callback from completing a fade while PAL's cached clock is parked.

The parameter-query adapter no longer returns invented loaded/playing/loop values
for selectors 0..4. Native scalar fields have their recovered meanings, including
byte loop bounds, source group, the full pause and loop words, and configured
channel gain. Missing streaming/source fields return explicit errors. The
zero-argument now-channel query returns the first playing group-3 index or -1;
it is not the number of active channels. The convenience `PalSoundStatus` enum
is explicitly separate from the native queued-command GetStatus result.

The command word is now retained independently of playback state. Native stream
initialization stores 5; ordinary loading stores 0. PlayFade returns after native
command 6 has become 5. StopFade publishes 7, transitions to 3 while fading and
acknowledges 5 when stopped. Pause/resume retain 9/10 until their fade/control
work completes, then acknowledge 5. Group-3 natural completion releases its
source and clears the command to 0. The portable mixer publishes these words
with its actual command processing; `GetStatus` and selector 13 no longer return
an invented enum or an unconditional unsupported error. It does not reproduce
the native worker's 10 ms message-wait cadence or 100000-iteration wait timeout.
Evidence: `audio-totsulover-state-machine.json`, worker `0x100B07C0`, thread
`0x100B0D90`; public control bodies in `audio-totsulover-exports.json`.

Master and channel gains retain their raw signed words; only group gains clamp
to 0..10000. Copies preserve the source record's group field, which controls
gain even when the copy lives in a different bank. Gain computation follows the
float32 group/channel multiplication, wrapping 32-bit master product, and
hundredths-of-decibel conversion. Invalid DirectSound gain retains the previous
buffer level; the native retry timeout is not reproduced.

These changes do not establish complete audio parity. Remaining implementation
work includes native ring-buffer read/write/play cursors, exact worker
acknowledgement timing, unsupported source formats,
EPEG, native resource addresses, and copied transient worker latches. Native
integer gain quantization during each fade tick remains different. The
already-established movie decoder/device-reset and hardware timing limitations
still apply. Earlier regression results are historical evidence only; further
regression-test work was stopped at the user's request.

`PalGetSoundBufferFormat` now returns a packed 18-byte native record for known
ordinary WAV/Ogg PCM buffers. The preallocated group-0/3 buffers report PCM,
two channels, 44100 samples/second, 176400 bytes/second, block alignment 4 and
16 bits/sample even before a file is loaded; this follows worker initialization
`0x100B0D90`, not the portable mixer's float output. Unknown/compressed/mixed
source formats produce an explicit adapter error. `PalSoundSetVolumeRatio`
retains and copies the original float field; later setter calls do not silently
amplify already-loaded PCM again.

EPEG header and raw audio reading were narrowed further in
`evidence/audio-epeg-stream-read.json`: reader `0x100B0330` copies file bytes
without a codec transform into the fixed stream buffers, but group 0 changes
its read position to -1 and zero-fills the tail after reaching its declared
end. Initial prefill `0x100B0580` also zeroes the first 128 bytes of each lock
region. The ordinary portable PCM loop is not equivalent to this protocol;
EPEG playback remains unimplemented instead of being advertised as decoded.
