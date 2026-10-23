# Current consolidated entry point

Read the [English ADR index](adr/README.md) before revisiting confirmed behavior.
It records evidence scope, implementation limits and concrete reopening triggers.

See [README.md](README.md) for the 2026-10-23 evidence, corrections, runtime/codec reports and remaining gaps. The historical inventory below is preserved; older IDB names are not semantic proof.

Latest implementation: [VM dispatch and raster FX](fx-vm-implementation.md).
The [pre-fix audit](vm-pal-shader-audit.md) records the original findings and
version correction.
The current fixture differs from the reference binaries. The reference release
has 290 PE exports, the debug DLL 288, and the fixture DLL 262; 215 is the
reference executable's PAL import count. The old 132-entry primary-table claim
below is not a valid whole-VM bound; the audit inventories the known 0..262 range.

# Reverse Engineering Index

## Binaries

| Binary | IDB | Session ID | Notes |
|--------|-----|------------|-------|
| `Game.exe` | `Game.exe.i64` | `game` | 32-bit PE, base 0x400000. ~750 functions. No debug symbols. (Converted from old split format on first idalib save.) |
| `PAL_d.dll` | `PAL_d.dll.i64` | `pal_d` | 32-bit PE, base 0x10000000. Debug version of PAL engine. 289 named exports, 13678 total functions. |
| `PAL.dll` | `PAL.dll.i64` | — | Release version. 215 exports. Game.exe imports from this at runtime. |

## Documents

| File | Contents |
|------|----------|
| `GAME_VM_RE.md` | Game.exe VM: opcode dispatch, extcall system, ScriptContext layout, key function addresses |
| `PAL_EXPORTS_RE.md` | PAL_d.dll exported functions, their purposes, and cross-binary relationships |
| `STRUCTS_RE.md` | Recovered struct definitions with field-level xref evidence: ScriptExecFrame, ScriptContext, BgmSlot, PalSpriteSlot, PalTask, PalSprite, ActionSlot |
| `ENGINE_CONTEXT_RE.md` | Complete ScriptContext field map: all known DWORD offsets with byte ranges, region summary, evidence addresses |
| `PARAMETER_PASSING_RE.md` | Calling conventions, VM stack protocol, ctx/task_data offset relationship, PAL bridge, BGM/sprite/action parameter passing |
| `IDB_CHANGELOG.md` | Per-session log of all IDB metadata changes applied |

## Architecture Summary

SoftPAL engine architecture:
```
Game.exe                PAL_d.dll
  WinMain                 PalInit
    ├─ PalInit ──────────► (registers callbacks, inits engine)
    ├─ PalRun ───────────► (main loop: input, update tasks, render)
    │     PalTaskCreate (for script task)
    │       VmScriptRunStep (0x44A2F0)
    │         gOpcodeDispatchTable[opcode](ScriptContext*)
    │           VmOpc_Extcall (0x42CF50)
    │             gExtcallCategoryTable[cat][idx](ScriptContext*)
    │               (sp_set → PalSprite*; bgm_play → PalSound*; ...)
    └─ PalEnd / PalUninit
```

## IDB Status

- `game`: ~616 renames, 29 comments, ~620 type edits, 9 struct/typedef declarations (ScriptExecFrame, PalInitParams, VmExtcallHandler, VmNode, VmAnimCb, VmAnimCb_TickFn typedef, PalSprite, SpriteMotionEntry, ScriptContext typedef). 26 saves. Last save: 2026-06-09 Session 7. All VmExtcall_*/VmOpc_*/VmEffect_*/VmTask_* handlers typed ScriptContext *ctx (~230 functions total). VmOpc_Save/SeLoad/SkipSet/SelectInit also fixed (confirmed ctx via field access).
- `pal_d`: 4 struct declarations (PalSprite/PalTaskNode/PalSoundSlot/PalInitParams, ordinals 258-261), ~30 function signatures, ~6 renames, ~5 comments. 4 saves. Last save: 2026-06-09 Session 4. Functions have full debug symbols (289 named exports). Block global at 0x103E1A30.

## Key Addresses (Game.exe)

| Address | Name | Purpose |
|---------|------|---------|
| 0x44A2F0 | `VmScriptRunStep` | Main VM execution loop (__thiscall on ScriptExecFrame) |
| 0x42CF50 | `VmOpc_Extcall` | Opcode 23: extcall dispatcher |
| 0x4BB1D0 | `gOpcodeDispatchTable` | Primary opcode handler table (132 entries) |
| 0x49C360 | `gExtcallCategoryTable` | Extcall category→handler_table mapping (24 entries) |

## Key Addresses (PAL_d.dll)

All named via debug symbols. Base 0x10000000. See `PAL_EXPORTS_RE.md` for full list.
