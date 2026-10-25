# PAL platform, lifecycle and diagnostic behavior

The 2026-10-23 audit records 160 native export instances: 66 in Totsulover, 48 in
the reference DLL and 46 in Koikake. Exact names, addresses, SHA-256 identity,
arguments, returns, pointer obligations and implementation mappings are in
[`contracts/platform_lifecycle.json`](contracts/platform_lifecycle.json).
Raw export bodies and the window/thread/wait/initialization helpers are in
[`evidence/pal-platform/`](evidence/pal-platform/). These are release PAL exports,
not Game.exe VM commands or Sena's host C API.

## Parameters, initialization and context access

All three profiles copy the same 108-byte, four-byte-aligned system parameter
record. It includes HINSTANCE, window/debug mode words, borrowed string/resource
pointers, style, frame rate, window and logical dimensions, capacities, callbacks
and renderer/window flags. Each field offset is recorded in the JSON. Three field
names remain semantic placeholders even though their offsets and widths are known;
their names must not be treated as recovered source declarations.

`PalDefaultCreate` zeroes that record, sets title `WindowTitle`, class `CLASS`,
60 FPS, texture capacity 256, random table length 2048, frame-pacing mode 2 and
aspect mode 1. Its default client/logical dimensions differ:

| Profile | Default dimensions | Engine allocation | Scratch pool |
| --- | --- | --- | --- |
| Koikake | 960x600 | `0x2A90` | `0x2000001` bytes |
| Reference | 1920x1080 | `0x31D0` | `0x8000001` bytes |
| Totsulover | 1280x720 | `0x4B30` | `0x8000001` bytes |

These defaults do not override the game executable's configuration. The game can
replace dimensions and must provide the startup callback before `PalInit`.
DefaultCreate's residual EAX differs by build and is not a confirmed return API.

`PalInit` rejects null parameters and zero client dimensions, registers the window
class, recenters positions outside the desktop bounds, creates the main window and
initializes the engine. Recentering changes the caller's input record. Totsulover
initializes from `WM_CREATE`; old profiles call the initialization helper after
window creation. GPU initialization can fall back to software and clear renderer
flag 8. Failures leave partial state; this is not a transactional initialization.

`PalGetSystemParameters` shallow-copies the current 108-byte record. Copied pointers
remain borrowed. `PalGetCore` returns the selected GPU/software renderer context,
not the whole PAL engine. `Pal3DIs` returns 0 or **8**, not 0 or 1. `GetMemoryPool`
and the newer `PalGetlpPool` return borrowed scratch storage. `PalGetFilePath` takes
no filename argument and returns a base directory string; older decompiler calls
that appear to pass a filename are stack-recovery artifacts.

## Threads, callbacks and shutdown

`PalRun` is a blocking lifecycle function. It creates a work thread and a render
thread, waits on their startup events, sets the running word and pumps the main
window's messages. The render thread calls the startup callback once; a zero result
requests shutdown. Its loop updates cached time and input, runs tasks while active
or when the background-run flag is set, paces rendering, draws and dispatches its
thread messages. Newer main-thread idle uses `Sleep(1)`; old profiles use
`WaitMessage` (the reference main loop uses Unicode message dispatch).

Both attach functions pass a cdecl callback and one 32-bit argument word. The
callback's return is ignored. `PalAttachWorkProcess` retries posting until accepted,
without timeout, and the work thread ignores null callback pointers. Main-window
dispatch calls the pointer unconditionally. The public attach behavior changes:

| Profile | `PalAttachProcess` |
| --- | --- |
| Koikake/reference | Retry asynchronous `PostMessageA`; return successful BOOL |
| Totsulover | Synchronous `SendMessageA`; return handler LRESULT, which is 0 |

Calling both versions through a generic success-boolean adapter changes observable
results and synchronization. The callback argument remains borrowed; queueing does
not copy an object that it points to.

`PalEnd` synchronously sends shutdown message `0x8069`. Its handler closes debug UI
and cursor state, stops work/render threads, restores IME and destroys the main
window. The thread stop helper sets a process-global shutdown latch, posts stop
messages and pumps messages while waiting. After 100 ten-millisecond waits it
attempts `TerminateThread` and continues waiting. These are observed native failure
mechanisms, not an instruction to reproduce unsafe thread termination in Rust.
An already-ended `PalEnd` leaves an incidental EAX value.

`PalUninit` frees engine resources and is separate from stopping the work/render
threads. Newer/reference clear the global context after freeing it; Koikake leaves
a dangling global pointer. Calling Koikake Uninit twice is unsafe. The reviewed
initialization paths do not reset the shutdown latch, so repeated process-global
initialization is not established as a supported native lifecycle.

## Waits and clocks

The PAL clock is a cached wrapping 32-bit millisecond value. `PalUpdateTime` samples
`timeGetTime`, while `PaltimeGetTime` only returns the cache. Repeated queries without
a frame update do not advance time.

`PalWait` creates a scheduler task, not an OS-thread sleep. Its first callback
installs the countdown callback; later callbacks decrement the signed bit pattern
unless it is `-1`. Reaching zero frees the task. A count of zero becomes `-1` on the
first countdown and then remains indefinite. A count of `-1` is indefinite from
the outset.

`PalWaitTime` captures the cached clock on its first scheduled callback, then
completes when unsigned wrapping elapsed reaches the duration. `PalWaitTimeEx`
uses the same comparison but takes a **parent task pointer** as argument two. Its
12-byte work record stores duration at +4 and the captured clock at +8. It does not
take a cancellation mask. Both functions return scheduler task pointers.

`PalFrameRateSet` writes its input without validation. The render loop evaluates
integer `1000 / fps`; zero can divide by zero. Newer PAL first displays a diagnostic
message, but still reaches that division. `0xFFFFFFFF` takes the one-millisecond
sleep branch. Frame-pacing mode 2 sleeps the remaining integer frame interval;
other modes sleep one millisecond. The setter's EAX is an incidental engine pointer.

## Window and desktop requests

Window-size, window-mode and aspect-mode exports queue render-thread messages,
retrying for at most 3000 milliseconds while pumping the caller's messages. Their
success result means the message was accepted, not that the requested D3D reset
succeeded. The render-thread mode handler also observes `PalWindowChangeEnable`;
a disabled request can be successfully queued and subsequently ignored.

Window mode zero is ordinary windowed. The downstream size handler preserves
logical aspect ratio: a changed 800x900 request with 1280x720 logical size applies
800x450. Requested height is used in the no-change comparison but is otherwise
recomputed from width using float division and truncation. Aspect-mode indices
directly index the native `[0,1,2,3]` tables without a bounds guard.

`PalDisableChangeWindowMode` is a separate newer global switch: the window procedure
uses it to suppress Alt+Enter and bypass the close confirmation. It does not replace
the render-thread enable word. `PalSetChangeWindowModeTime` has no input timestamp;
it samples the OS clock into its own global slot.

`PalGetWindowSize` accepts either optional output and returns 0 only when both are
null. In fullscreen it returns the presentation rectangle extents. Position getters
have optional X/Y pointers but no stable return contract. Setting position writes
stored X/Y before `SetWindowPos`, retaining those writes even if Windows rejects
the move. Centering uses desktop right/bottom with arithmetic shifts, ignoring the
desktop left/top. Totsulover's desktop override affects centering and initialization
without itself immediately resizing an existing window.

## Debugging and logging

The native debug dialogs are 36-byte owned records. They hold ID, visible word,
mode byte, borrowed external data pointer, count, log handle and next pointer.
Duplicate IDs fail creation. Mode byte 1 creates a per-dialog log file; byte 2
selects two-column output. Newer creation honors a saved `view` value and defaults
to 300x230; old creation uses 100x100 and does not read that visibility key.

`PalDebugSetData` reads a pointer from the caller's pointer-to-pointer, stores it
without copying its elements, creates numbered list rows and **returns 0 even on
success**. Dialog deletion does not free this external data. Deleting the displayed
log clears list items but does not truncate the on-disk log. `PalBeginUpdate` and
`PalEndUpdate` call `LockWindowUpdate` for the debug list; they are not graphics
frame begin/end operations.

DebugPrintf and both ErrorLogPrintf functions are cdecl varargs. The second fixed
DebugPrintf argument is a borrowed first-column label, not a color or integer
format argument. The newer implementation suppresses hidden dialogs and caps ID 10
to 2000 displayed rows; older implementations do neither. ID 0 bypasses the global
suppression bit in reference/newer, but not Koikake. Formatting uses fixed temporary
buffers and checks length only afterward, so native overflow cases must not become
unsafe portable formatting.

`PalErrorLogPuts` takes no string argument: it reads GetLastError, formats the OS
message, emits it to the debugger and error log. File-write return values are not
propagated by the log exports. Newer debug setters retain raw pointer/number words,
and the decorated DebugDump is an empty void function. `PalSaveScreenShot` is a
constant-zero release stub; no screenshot path is implemented by that export.

## Related render consumers

The requested follow-up for `PalSetPow` is resolved in the saved field-reference
and consumer evidence. Its low byte forces power-of-two GPU texture allocation.
The same round-up is also selected when D3D texture capabilities require it. It is
not a gamma/exponent effect. Ordinary texture creation/loading retains logical
dimensions while allocating the next power-of-two physical sizes; the scene render
target additionally makes both physical dimensions the larger square side.

The three thumbnail capture helpers are also saved in `*-capture.json`. They copy
the centered logical scene from a GPU staging surface to caller BGRA storage,
force alpha to 255, and leave extra destination space untouched. Destination width
and height must cover the logical size. Surface/copy/lock errors return 0; unlock
or release failures do not change a successful return. The first source-row offset
uses the returned pitch, but later rows advance physical width times four. These
helpers feed the separately audited thumbnail exports.

## Limits of the coverage claim

Every assigned export has a body-backed record, with major callback/message paths
traced beyond its boundary. Incidental return registers, a constant stub's argument
arity, unnamed parameter semantics, driver behavior and untraced private helpers
remain explicitly distinguished. This inventory is not a claim of portable host
parity or complete reconstruction of Windows and D3D. Confirmed callable exports
must enter the central reviewed ABI manifests before native adapter implementation.
