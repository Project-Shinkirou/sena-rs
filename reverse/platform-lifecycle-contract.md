# Platform and lifecycle contract acceptance audit

The authoritative per-export records are
[platform_lifecycle.json](contracts/platform_lifecycle.json). The broader narrative
is [PAL platform and lifecycle behavior](pal-platform-lifecycle-behavior.md).
This audit distinguishes exported behavior, remaining native uncertainty and
portable implementation differences.

## Independent inventory check

Direct PE32 export parsing and independent SHA-256 recomputation matched all 160
platform records to the actual DLLs: 46 Koikake, 48 reference and 66 Totsulover.
The complete six-domain union covers the actual 262/290/369 named exports without
missing names or address/hash mismatches. Decorated exports remain decorated in
the inventory. None of these counts measures implementation parity or closure of
the complete private call graph.

## Corrected task and clock interpretation

`PalWaitTimeEx` argument 2 is a **relative task pointer**. The export forwards it
to PalTaskCreate with flags `0x10001`, which inserts after that task in the same
sibling list. It does not create a child task. Null selects the executing task.
The older narrative's phrase "parent task pointer" describes forwarding too
loosely and is superseded by this correction and the updated JSON.

The 12-byte work record is reserved word +0, duration +4 and captured clock +8.
The first callback samples cached PAL time; later callbacks compare unsigned
wrapping elapsed against duration. PalWaitTime has the same two phases with an
8-byte record. PalWait also has an initialization callback; its countdown skips
only -1, decrements other 32-bit patterns with wrap, and frees only on exact zero.
An initial zero becomes -1 indefinitely.

The native clock cache stores wrapping timeGetTime DWORDs. Engine elapsed time
must therefore be truncated to u32, not saturated at `u32::MAX`; saturation would
freeze waits and animation after approximately 49.7 days. The root implementation
follow-up corrected that caller while preserving wrapping elapsed comparisons.

## Native uncertainties that remain

- Source returns remain unresolved for `DebugSetActionList`, `GetMinWindowSize`,
  `PalDefaultCreate`, `PalDisableChangeWindowMode`, `PalEnd`, `PalFrameRateSet`,
  `PalGetDesktopRect`, `PalSetChangeWindowModeTime`, `PalSetDesktopRect` and
  `PalWindowGetPos`. The affected builds and observed register paths are listed
  individually. Their side effects have been reviewed; an incidental register
  value is insufficient to invent a public C declaration.
- Totsulover `PalSaveScreenShot` is a constant-zero release stub. The body proves
  no screenshot or file write, but cannot prove its unused argument arity.
- The system parameter record is exactly 108 bytes, with known offsets and
  widths. The semantic name of +68 (default 512), the complete meaning of renderer
  parameter +84, and the semantic name of +104 (default 1) remain unresolved.
  In particular, +68 must not be renamed task capacity merely because both have
  a default of 512: Totsulover initialization writes the task pool capacity as a
  separate literal 512 in `0x100A34F0`.
- Process callbacks receive one cdecl argument and their result is ignored;
  shutdown callback result is likewise ignored. A portable void consumption
  contract is not proof that every original callback source declaration was void.
- `PalDebugSetSaveThumbnail` stores a complete word without dereferencing it.
  Its downstream semantic type is not established by the reviewed setter.

## Downstream closure and external boundaries

Saved evidence traces the major window-message consumers, work/render thread
loops, first wait callbacks, shutdown signaling, renderer-mode requests and
initialization. It does not certify every private helper reachable from them.
No complete Windows-message interleaving or allocation-failure exploration was
performed. Queue acceptance is distinct from completed resize/device reset.

Driver behavior, COM/Win32 return values, IME state and host window placement are
external dependencies rather than reconstructed PAL implementation. Native
shutdown can attempt TerminateThread; Koikake Uninit leaves a dangling global
context; malformed FPS zero can divide by zero; several native formatting paths
can overflow fixed buffers. Those observed hazards are documented, not reproduced
as unsafe host behavior. Reinitializing after global shutdown is not established
as a supported native lifecycle.

Portable platform adapters and source ABI are separate from the native exports.
Desktop compilation and the game's actual FFI smoke do not certify Windows,
Android, iOS, Switch or Vita lifecycle parity. The font smoke concern during this
pass was resolved through continued actual playback: after stopping further
clicks, the full ADV line appeared by frame 3659 and remained visible through
4199; the frame-3599 fragment was a reveal boundary, not failed glyph rendering.
