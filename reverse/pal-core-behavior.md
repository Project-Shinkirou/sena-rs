# PAL core behavior contracts

The exact export/build records are in `contracts/core.json`. They cover list,
random, memory, file, search-path, task and reference-language exports. Each row
links a native body and its binary SHA-256. This is a static native contract
review, not a claim that every portable adapter has equivalent behavior.

## Lists and ownership

A native list is a 12-byte sentinel followed by 12-byte nodes: data at +0, tag at
+4, next pointer at +8. Push prepends; PushLast appends. Delete matches data,
whereas Pop matches tag. Both remove only the first match. Nodes own no payload.
Release frees nodes and sentinel, leaving payload cleanup to the caller.

Find and FindNext return a **predecessor cursor**, not a matching node. FindNext
first reads cursor.next to recover the current match and its tag, then searches
after it. A portable vector index is not a stable substitute across mutation.
Koikake PopFirst dereferences a null list; reference and Totsulover explicitly
return zero. Such invalid native pointers should produce a safe portable error,
not an intentional host crash.

## Random streams

The Windows CRT generator advances unsigned state as
`state = state * 214013 + 2531011` and returns `(state >> 16) & 0x7fff`.
Startup seeds it with the low 32 bits of Unix time, then fills the default 2048
DWORD ring. PalRandom normally consumes the CRT stream; in debug mode it consumes
the ring. PalRandomEx always consumes the ring. User-provided ring values retain
all 32 bits. Reset refills from the current CRT state, without reseeding.

SetSeed resets the cursor, copies the bounded prefix, leaves the tail unchanged,
and returns copied_count - 1. GetSeed leaves the cursor alone and returns the
copied count. Reset has no established source return type: its incidental EAX
differs between builds. The Rust generator and startup seed now follow the
recovered algorithm; explicit-seed tests remain deterministic.

## Files and archive search

The output file range is three DWORDs: start byte, end byte, size byte count.
Loose-file success is `{0, size, size}`. An archive hit is `{offset, offset+size,
size}` and returns the open archive handle already positioned at the entry.
Callers own that OS handle. FileSetFilePointer consumes five stack words, ignores
the fourth, does not add archive start, and returns 1 even if Win32 seeking fails.
Whether the source declaration groups distance-low/high as one 64-bit argument
is unresolved; the machine argument layout is explicit in the contract.

Each base path probes a loose file before its `.pac` archive. Paths are prepended,
so the last registered base wins. The native basename/extension conversion
uppercases ASCII and skips the byte following any negative signed byte. Archive
lookup uses the first-byte bucket and 40-byte directory entries. Read failures,
malformed archive tables and overlong native paths are not comprehensively
guarded by the original. Portable bounded parsing should retain safe failures.

The reference DLL first searches CN paths when its language field is below 2,
TC paths when equal to 2, and skips language paths otherwise. It then searches
base paths and the executable directory. GetLanguage reports 1 for unset/null
context; SetLanguage maps 0 to 1 and otherwise stores the supplied value. These
native language exports must not be confused with similarly named VM font calls.

GetFullPath has distinct behavior from opening a resource: it can format an
archive member as a nominal loose path, uses only the reference base list, and
accepts a byte length equal to capacity without adding a terminator. Empty-list
paths differ by build (Koikake has an uninitialized handle local; reference starts
at zero; Totsulover starts at -1). These findings come from instructions, not
executed invalid-state tests.

## Tasks and callback order

The native 44-byte task layout is recorded field by field in `core.json`.
Callbacks receive one cdecl stack argument, the work pointer. Normal processing
ignores the return; ChangeNextImm forwards the raw EAX result. Do not infer an
integer source return type solely from that forwarding instruction.

Create reserves a task plus child sentinel, allocates zeroed work, and links by
the low flags word: 1 after the chosen task; 2 after its child sentinel. Children
are processed before the parent. A `0x10000` flag stops subsequent normal sibling
callbacks while still allowing pending sibling destructors to be collected.
Free marks state 2, installs the destructor as current callback and recursively
marks descendants; the scheduler later runs cleanup and releases work. It is not
an immediate deallocation call. GetState reports 2 whenever current callback
equals destructor, even if the stored state byte says something else.

Messages are a single overwrite slot. GetMessage reads and clears it. The
selected task-data context is separate from the executing-task context, so a
null GetTaskData argument does not necessarily select the current task.
ReProcess starts from the supplied task's predecessor and can run later siblings,
then restores the executing-task context. Setter EAX values vary by build and
are explicitly left unresolved as source returns.

Pool exhaustion and residual cleanup contain unsafe native paths. They are
documented rather than reproduced as host memory faults. Portable allocation
failure must unwind safely; that is an intentional safety difference.

## Validation boundaries

`contracts/generate_inventory.py` reconciles exact export names, addresses,
hashes and captured bodies. `--check` detects stale coverage. Machine ABI,
source declaration, downstream behavior and portable parity remain independent
claims. Native bodies, helpers, initialization and CRT evidence are retained in
`evidence/pal-core-*.json`.
