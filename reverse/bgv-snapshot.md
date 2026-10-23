# Portable BGV snapshot persistence

BGV uses the independent eight-lane wrapper recovered in `bgv-playback.md`; it is not an ADV voice playlist or BGM slot. Its portable save state is now independent too.

## Captured state

Each loaded active BGV lane retains its resolved resource name, including when the original script value was a mutable dynamic string. Snapshot capture records the slot (0–7), resource name, loop flag, paused flag, and optional remaining stop-fade deadline. Expired stop requests are omitted. The per-lane audio gain is always 100% in the native BGV loader, so there is no independent channel-volume parameter to save. BGV's current user volume/mute settings remain preferences and are applied when replaying, like existing BGM settings.

The version-10 portable suffix follows Tips and contains:

1. `u32` track count, at most eight.
2. Per track: signed `i32` slot; length-prefixed UTF-8 resource name bounded by `SAVE_NAME_CAP`; one byte of loop/pause flags (bits 0/1); signed remaining stop time in milliseconds (`-1` means no stop deadline).

The reader rejects duplicate/out-of-range slots, invalid flags, invalid UTF-8, oversized names/counts, and stop values below -1.

## Restore lifecycle

Restoring a snapshot queues the saved BGV list. Audio mutation occurs at the beginning of the next `run_frame`, before early returns for a parked ADV wait. It releases only the outgoing BGV namespace, applies the current BGV preference gain, loads each saved resource, and restores its loop/pause state. A pending stop is scheduled relative to the new playback clock. The operation runs once; snapshots captured while replay is pending preserve the incoming list.

If audio is not available, the pending list is retained. If audio exists but resources are unavailable, the outgoing BGV is stopped immediately and the saved list remains queued until resources are available. Empty snapshots still stop outgoing BGV.

Versions 1–9 contain no BGV list. They therefore queue an empty list instead of inventing tracks from the outgoing scene. Original-engine saves do the same: the native sound wrapper remains unmapped. Restoring BGV does not stop or modify ADV voice handles, character volume, autoplay voice gating, or voice autopan.

## Tests and limitations

Seven focused `runtime::bgv` tests pass, including four persistence tests using generated WAV resources outside game directories. They cover an encoded version-10 round trip, mutable source-name reuse, paused loop restore at a parked wait, one-shot replay, current preference gain, ADV-voice independence, legacy versions 1–9, deferred resource availability, relative stop deadlines, and malformed lane lists.

Playback restarts at the beginning of the clip; decoder/sample offsets are not serialized. A partially completed stop fade resumes with its remaining duration, but the instantaneous fade-envelope gain is not captured. The native original-save sound wrapper is still unknown; this portable suffix does not establish original-engine save compatibility.
