# Parameter Passing and Calling Conventions

Xref-backed analysis of how parameters flow between Game.exe components and across the Game.exe/PAL_d.dll boundary.

---

## Calling Conventions Summary

| Convention | Use case |
|-----------|----------|
| `__cdecl` | Extcall handlers, most Game.exe internal helpers |
| `__thiscall` | VmScriptRunStep, state-reset helpers that take ctx as ECX |
| `__fastcall` | Utility helpers (VmStr_GetFilename, VmSnd_LoadOgg) |
| `__stdcall` | WinMain (0x405C30) and PAL imports |

---

## 1. Extcall Handler Convention

Every extcall handler has the prototype:

```c
int __cdecl handler(unsigned int *ctx);
```

`ctx` = `(unsigned int *)(PalTaskGetTaskData(0) + 824)`

- Passed via the stack as the single argument.
- Return value: 1 = success/continue; 0 = error/stop.
- ctx is a DWORD-array pointer into the main game task data block.

Evidence: decompile of `VmOpc_Extcall` (0x42CF50), `VmExtcall_Wait` (0x444F40), `VmExtcall_TextInit` (0x441400), `VmExtcall_SpSet` (0x4299A0), `VmExtcall_BgmPlay` (0x40C4B0).

---

## 2. VM Stack Argument Passing

Script instructions push values onto the VM stack before calling an extcall. Handlers pop values off the stack in LIFO order:

```c
// Pop top of stack into local variable
v_idx = ctx[184628];          // stack_top_idx (byte offset 738512 from ctx)
ctx[184628] = v_idx - 1;      // decrement top
local_val = ctx[(v_idx - 1) + 177646]; // stack[top-1], byte base 710584
```

Stack layout:
- `ctx[184628]`: stack top index (0-based, points one past the top element)
- `ctx[177646 + i]`: stack element i (i = 0..63)
- Maximum 64 elements.

### Standard pop sequence (confirmed in all major handlers):

```c
unsigned int top = ctx[184628];
if (top - 1 < 0) PalDebugPrintf(0,0,"not stack");
ctx[184628] = top - 1;
unsigned int val = ctx[(top-1) + 177646];
```

### Return value writeback:

```c
// ctx[184629] = extcall_arg = destination variable index
ctx[ctx[184629] + 177710] = result;
// variable[dst_var_idx] = result
```

Variable array: `ctx[177710 + var_idx]` for var_idx = 0..65535 (65536 VM variables).

Evidence: `VmExtcall_BgmPlay` (0x40C4B0) line `ctx[ctx[184629] + 177710] = v4`.

---

## 3. Extcall Argument Register

`ctx[184629]` (byte offset 738516) is written by `VmOpc_Extcall` before calling the handler:

```c
ctx[184629] = instr2;  // second DWORD after extcall opcode = destination operand
```

Handlers use this to write back return values without an explicit return from the handler prototype.

---

## 4. PAL Import Stub Calling Convention

Game.exe calls PAL.dll functions through an import stub layer:

```
Game code → import stub (e.g. 0x4500BE: jmp [__imp_PalInit]) → PAL.dll export
```

All PAL imports are `__stdcall` (callee cleans the stack). Key import addresses:

| IAT entry | Import stub | Function |
|-----------|-------------|----------|
| 0x4BBB00 | 0x4500BE | PalInit |
| 0x4BBB04 | 0x4500EE | PalRun |
| 0x4BBB08 | — | PalEnd |
| 0x4BBBCC | — | PalTaskGetTaskData |
| 0x4BBAD4 | — | PalDebugPrintf |
| 0x4BBAC4 | — | PaltimeGetTime |

The full list of 211 used imports is in `PAL_EXPORTS_RE.md`.

---

## 5. ctx vs. task_data Offsets

Two distinct pointer bases appear in the codebase:

| Name | Value | Notes |
|------|-------|-------|
| `task_data` | `PalTaskGetTaskData(0)` | Raw PAL task data block base |
| `ctx` | `task_data + 824` | Typed as `unsigned int *`; all handler ctx offsets are from here |

Functions that take ctx as `int this` (byte pointer) use byte offsets directly:
- `this + 738512` = `ctx[184628]` (stack_top_idx)
- `this + 710584 + 4*i` = `ctx[177646 + i]` (stack values)
- `this + 804060` = `ctx[201015]` (PC)

Functions that take ctx as `unsigned int *` use DWORD indices directly:
- `ctx[184628]`, `ctx[201015]`, etc.

---

## 6. VmState_ResetScheduling Parameter Access (0x44AC90)

Takes `ctx` as `__thiscall` ECX. Uses byte offsets (int this):

| Byte offset | DWORD index | Purpose |
|-------------|-------------|---------|
| this+804232 | ctx[201058] | (cleared) scheduling sub-state |
| this+804236 | ctx[201059] | (cleared) scheduling sub-state |
| this+804248 | ctx[201062] | (cleared, byte) scheduling mode byte |
| this+804252 | ctx[201063] | (cleared) scheduling mode dword |
| this+804256 | ctx[201064] | (cleared) scheduling mode flag |
| this+804264 | ctx[201066] | (cleared) history_skip active |
| this+4252   | ctx[1063]   | task handle — freed if nonzero |
| this+4      | ctx[1]      | text active flag — passed to VmText_Reset |
| this+8      | ctx[2]      | cleared to 0 |
| this+48     | LOWORD(ctx[12]) | set to -1 |

---

## 7. Action Slot Stride Formula

Action handler (0x40B6E0, VmExtcall_ActionRun) computes per-slot base:

```c
_DWORD *slot = (_DWORD *)(296220 * action_idx + ctx_bytes);
// active_flag: slot[204567] = *(ctx_bytes + 296220*idx + 818268)
// param:       slot[204572]
// run_flag:    slot[204573] = 1
```

Action slot stride: 296220 bytes (74055 DWORDs). Up to 16 slots.
Slot i active_flag DWORD index from ctx: `204567 + 74055 * i`.

---

## 8. Save State Parameters (VmSaveState_AdvStack, 0x42F850)

Takes ctx as `_DWORD *this`:

| Offset | DWORD index | Purpose |
|--------|-------------|---------|
| this[2192] | ctx[2192] | save block base |
| this[2193] | ctx[2193] | save param (frame size?) |
| this[2194] | ctx[2194] | save param |
| this[2195] | ctx[2195] | save active flag |
| this[2196] | ctx[2196] | save param (width?) |
| this[2197] | ctx[2197] | save param (height?) |

Thumbnail creation: `PalThumbnailCreate(ctx[2196], ctx[2197], ctx[2193], ctx[2194])`.
Conditional: `PaltaskGetTaskData(0)[4]` and `PalTaskGetTaskData(0)[9592/4]` check task flags.

---

## 9. Inline String Storage

`VmStr_GetFilename` (0x44B1E0) handles resource IDs with bit 0x10000000 set:

```c
// inline string at: PalTaskGetTaskData(0) + 2047 * (id & 0x0FFFFFFF) + 772120
char *str = (char *)(PalTaskGetTaskData(0) + 2047 * slot_idx + 772120);
```

String slot size: 2047 bytes. Slot index: `id & 0x0FFFFFFF`.
Byte offset 772120 from task_data = `772120 - 824 = 771296` from ctx.
From ctx: `ctx_bytes + 771296` = `ctx[192824]` (first byte of inline string area).

---

## 10. VmScriptRunStep Context Key Accesses (0x44A2F0)

`this` = ScriptExecFrame*, `v2` = ctx (DWORD*):

| ctx index | byte offset | Purpose | Evidence address |
|-----------|-------------|---------|-----------------|
| v2[2208] | 8832 | mode flag (if nonzero: skip thumbnail) | 0x44a44c |
| v2[2200] | 8800 | saved PC | 0x44a46b |
| v2[2210] | 8840 | history counter | 0x44a471 |
| v2[22083] | 88332 | history gosub target point | 0x44a499 |
| v2[2211] | 8844 | mirror of 2210 (saved) | 0x44a4be |
| v2[2207] | 8828 | effect active flag (→1) | 0x44a4d5 |
| v2[3240] | 12960 | effect timer start (PaltimeGetTime) | 0x44a4e5 |
| v2[3241] | 12964 | effect timer duration (100 ms) | 0x44a4eb |
| v2[201056] | 804224 | saved PC (debug/trace) | 0x44a349 |
| v2[201057] | 804228 | history list (PalList*) | 0x44a40e |
| v2[201064] | 804256 | scheduling mode flag | 0x44a3d6 |
| v2[201066] | 804264 | history skip active | 0x44a34f |
| v2[201067] | 804268 | history skip end PC | 0x44a358 |
| v2[201016] | 804064 | script base/relative offset | 0x44a54c |

---

## 11. BGM Slot Layout

BGM slot stride: 31 DWORDs (124 bytes). 2 slots, base at `ctx[164206]` (byte offset 656824).

| Slot field index | Purpose |
|-----------------|---------|
| slot[0] | PAL sound handle |
| slot[1] | sound resource ID / name index |
| slot[2] | flags (bit 0 = mono/stereo, bit 1 = ?, bit 4 = loop, etc.) |
| slot[3] | volume (100 = full) |
| slot[8..23] | filename buffer (64 bytes, from VmStr_GetFilename) |
| slot[24] | loop flag (flags & 0x10) |
| slot[25] | fade-in time |
| slot[26] | fade-out time |
| slot[27] | additional param |

Evidence: `VmExtcall_BgmPlay` (0x40C4B0).

---

## 12. PAL Threading Model and Task Dispatch

PalRun creates two worker threads:
- **Update thread** (`sub_10237C00`): message-pump loop, dispatches task callbacks
- **Render thread** (`sub_1011ED79`): rendering operations

Task dispatch via Windows messages:
- `WM 0x806A`: update thread calls `Msg.wParam(Msg.lParam)` — universal callback mechanism
- `WM 0x8064`: exit signal

Main script task creation (PalInitParams[19] callback → VmTask_CreateMain → VmTask_Main):
```
PalInit() registers callback at PalInitParams[19] = VmTask_CreateMain (0x405380)
  → PalTaskCreate(VmTask_Main) → creates task node, allocates 5.5 MB data buffer
  → VmTask_Main loads system.dat into task data buffer
  → Sets ctx fields from saved state
  → Main loop posts WM 0x806A with wParam=VmTask_Main per-tick callback
    → Update thread calls VmScriptRunStep (0x44A2F0)
```

Block global (PAL_d.dll, 0x103E1A30) key fields for threading:
- Block[3] (+12): running flag (1=run, 0=quit)
- Block[78] (+312): render thread HANDLE
- Block[79] (+316): update thread HANDLE  
- Block[80] (+320): render event HANDLE
- Block[81] (+324): update event HANDLE
- Block[2] (+8): main window HWND

system.dat task data header (at task_data[0..0x330]):
- [+2*4=8]: window_mode → PalWindowChangeMode arg
- [+4*4=16]: effects_enabled → PalEffectEnable arg  
- [+9*4=36]: cursor_hide_time → PalCursorSetAutoHideTime arg
