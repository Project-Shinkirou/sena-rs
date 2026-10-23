# Core contract acceptance audit

This is an independent check of [the core contract](contracts/core.json), not a
claim that the complete engine has been reconstructed. The original narrative is
[PAL core behavior](pal-core-behavior.md).

## Export inventory

On 2026-10-23, the PE32 name, ordinal and function-RVA arrays were read directly
from the three DLL files. Their SHA-256 values were recomputed independently of
the saved interface inventories. Every core record matched its exact exported
name, address and binary hash. No core record named a nonexistent export.

| Build | Core export instances | Reviewed machine ABI | Unresolved source ABI |
| --- | ---: | ---: | ---: |
| Koikake | 36 | 24 | 12 |
| Reference | 40 | 28 | 12 |
| Totsulover | 39 | 26 | 13 |

Across all six domain contracts, the independently read DLLs contain 262, 290 and
369 exact named exports respectively. Every name has a contract record; there
are no missing names or address/hash mismatches. This verifies the inventory's
921 export boundaries. It does not verify every private callee, every possible
OS result, the Game.exe VM table, or the portable implementation.

## Resolved task details

The task and child sentinel are distinct 44-byte native nodes. Children run
before the parent during ordinary traversal. Low flags word 1 inserts after the
relative task in its sibling list; 2 inserts after its child sentinel; other
values leave the task unlinked. A null relative task selects the executing task.
The separately selected work-data context is not automatically the executing task.

Free changes the current callback to the destructor and marks descendants. A
later scheduler visit writes state zero **before** calling cleanup, with the work
block still available, then unlinks and releases it. A caller that merely writes
state 2 without calling Free therefore dispatches the current callback, not an
independently substituted destructor. State bytes other than 1 and 2 skip callback
execution but retain child traversal and blocking behavior. GetState reports 2
whenever current callback equals destructor, regardless of the state byte.

The post-blocking cleanup loop deliberately does not recurse into later sibling
children. Deleting a pending sibling there clears its child sentinel and can
orphan its descendants. This is visible in Koikake `0x100C8250`, reference
`0x100B7B80`, and Totsulover `0x100BDED0`, in their saved `pal-core-*-helpers.json`
files. Recursively freeing those descendants would be a behavioral change.

The Rust adapter now registers callback words explicitly as callable Rust
functions. Normal and cleanup dispatch ignore the callback result;
`native_change_next_immediate` installs the new callback, invokes it immediately,
and forwards its raw 32-bit result. Live sibling lists preserve same-pass
execution of tasks inserted after the current task. Work access survives the
state-zero cleanup callback. None of this executes original x86 machine code or
establishes the callback's original C return type. The machine/portable distinction
and reviewed declarations are in [task32.json](abi/task32.json), included by the
central generator.

## Concrete remaining uncertainty

These are source-ABI questions, not missing export bodies:

- `PalFileSetFilePointer`: five machine stack words are known. Whether the source
  grouped distance-low/high as a single 64-bit argument is not established.
- `PalListRelease`, `PalMemoryFree`, `PalRandomReset`, `PalTaskChangeNext`,
  `PalTaskChangeState`, `PalTaskReProcess`, `PalTaskReset`, `PalTaskSetMessage`,
  and `PalTaskSetTaskData`: incidental EAX/AL values do not establish a stable
  source return type. Build differences are recorded individually.
- `PalTaskCreate` and `PalTaskChangeNextImm`: callback calling convention and the
  single work-pointer argument are known. Normal scheduling ignores the result;
  immediate dispatch forwards raw EAX. The callback's original source return
  type remains unknown.
- Totsulover `PalListPrint`: caller-backed source return type is not established.

Malformed native inputs are a separate category. Pool-allocation failures,
residual task cleanup, unchecked list pointers, malformed archive directories,
and oversized paths can reach invalid memory. Static instructions are retained;
no native crash experiments or deterministic behavior after undefined memory
access are claimed. Safe portable rejection is intentional.

## Implementation and validation limits

The portable pool embeds child lists and permits 512 task objects; the native
pool consumes separate task/sentinel slots, so exhaustion is not equivalent.
Opaque callback words must be registered; unregistered callbacks cannot execute.
A still-linked state-zero Rust object is not reused as another task before
release, avoiding the native corrupt-list case. Native OS handles and addresses
remain distinct from portable generation handles and resource objects.

The callback follow-up was compile-checked with
`cargo check -p pal-vm --no-default-features --lib`; no new regression tests were
added after the user's instruction to stop adding them. Earlier wait/animation
checks do not certify the later callback implementation. Full engine parity is
not established by either this inventory or the compile result.
