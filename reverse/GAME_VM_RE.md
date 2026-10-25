# Game.exe VM Reverse Engineering Notes

## Instruction Format

Each script instruction is a 4-byte DWORD:
```
[HIWORD = 0x0001][LOWORD = opcode_number]
```
- `HIWORD == 1` → valid primary opcode
- `LOWORD` → opcode index into `gOpcodeDispatchTable`
- Example: `0x00010017` = opcode 23 (extcall)

Extcall instructions are followed by two more DWORDs:
```
[0x00010017]           ; opcode 23 (extcall)
[category<<16 | index] ; e.g. 0x00030002 = cat3:idx2 = sp_set
[argument_dword]       ; category-dependent argument / flags
```

## Primary Opcode Dispatch

**Table**: `gOpcodeDispatchTable` at `0x4BB1D0`
**Dispatch**: `gOpcodeDispatchTable[(unsigned __int16)instruction](ScriptContext*)`

### Opcode Map (core opcodes 1-33)

| # | Name | Handler |
|---|------|---------|
| 1 | mov | `VmOpc_Mov` (0x42D3E0) |
| 2 | add | `VmOpc_Add` (0x42D3B0) |
| 3 | sub | `VmOpc_Sub` (0x42D380) |
| 4 | mul | `VmOpc_Mul` (0x42D350) |
| 5 | div | `VmOpc_Div` (0x42D320) |
| 6 | bitand | `VmOpc_Bitand` (0x42D260) |
| 7 | bitor | `VmOpc_Bitor` (0x42D230) |
| 8 | bitxor | `VmOpc_Bitxor` (0x42D200) |
| 9 | jmp_point | `VmOpc_JmpPoint` (0x42D020) |
| 10 | jf_point | `VmOpc_JfPoint` (0x42CFB0) |
| 11 | gosub_point | `VmOpc_GosubPoint` (0x42CDE0) |
| 12 | eq | `VmOpc_Eq` (0x42D1D0) |
| 13 | ne | `VmOpc_Ne` (0x42D1A0) |
| 14 | le | `VmOpc_Le` (0x42D170) |
| 15 | ge | `VmOpc_Ge` (0x42D140) |
| 16 | lt | `VmOpc_Lt` (0x42D110) |
| 17 | gt | `VmOpc_Gt` (0x42D0E0) |
| 18 | lor | `VmOpc_Lor` (0x42D0A0) |
| 19 | land | `VmOpc_Land` (0x42D060) |
| 20 | lnot_slot | `VmOpc_LnotSlot` (0x42CF10) |
| 21 | end | `VmOpc_End` (0x42CEB0) |
| 22 | nop | `VmOpc_Nop` (0x42CEA0) |
| 23 | extcall | `VmOpc_Extcall` (0x42CF50) — see Extcall section |
| 24 | ret | `VmOpc_Ret` (0x42CC20) |
| 25 | reset_adv | `VmOpc_ResetAdv` (0x42CBF0) |
| 26 | mod | `VmOpc_Mod` (0x42D2F0) |
| 27 | shl | `VmOpc_Shl` (0x42D2C0) |
| 28 | shr | `VmOpc_Shr` (0x42D290) |
| 29 | neg_slot | `VmOpc_NegSlot` (0x42CED0) |
| 30 | pop | `VmOpc_Pop` (0x42CBA0) |
| 31 | push | `VmOpc_Push` (0x42CB50) |
| 32 | pack_args | `VmOpc_PackArgs` (0x42CAA0) |
| 33 | drop_args | `VmOpc_DropArgs` (0x42CA50) |

### Opcode Map (message opcodes 35-37)

| # | Name | Handler |
|---|------|---------|
| 35 | create_message | `VmOpc_CreateMessage` (0x42DDD0) |
| 36 | get_message | `VmOpc_GetMessage` (0x42DD60) |
| 37 | get_message_param | `VmOpc_GetMessageParam` (0x42DCF0) |

### Opcode Map (save/load opcodes 40-78)

These also serve as the extcall category 10 (save/load) handler table.
`gExtcallTableCatA_Save` = `gOpcodeDispatchTable[40]` = 0x4BB270.

| # | Name | Handler |
|---|------|---------|
| 40 | save | `VmOpc_Save` (0x433D10) |
| 41 | load | `VmOpc_Load` (0x433A60) |
| 42 | save_set_title | `VmOpc_SaveSetTitle` (0x4339B0) |
| 43 | save_data | `VmOpc_SaveData` (0x433690) |
| 44 | save_set_thumbnail_size | `VmOpc_SaveSetThumbnailSize` (0x433350) |
| 45 | thumbnail_set | `VmOpc_ThumbnailSet` (0x432F40) |
| 46 | savetitledraw | `VmOpc_SavetitleDraw` (0x4328C0) |
| 47 | save_set_font_size | `VmOpc_SaveSetFontSize` (0x432860) |
| 48 | getsaveday | `VmOpc_GetSaveDay` (0x433420) |
| 49 | is_save | `VmOpc_IsSave` (0x432620) |
| 50 | getsaveusermemory | `VmOpc_GetSaveUserMemory` (0x432480) |
| 51 | savepoint | `VmOpc_SavePoint` (0x4320C0) |
| 52 | save_thumbnail_mosaic_set | `VmOpc_SaveThumbnailMosaicSet` (0x432EE0) |
| 53 | savetimedraw | `VmOpc_SaveTimeDraw` (0x431C70) |
| 54 | savedaydraw | `VmOpc_SaveDayDraw` (0x4318C0) |
| 55 | save_set_text_rect | `VmOpc_SaveSetTextRect` (0x431330) |
| 56 | savetextdraw | `VmOpc_SaveTextDraw` (0x4313C0) |
| 57 | get_new_savefile | `VmOpc_GetNewSavefile` (0x4312F0) |
| 61 | setsavetext | `VmOpc_SetSaveText` (0x4338B0) |
| 62 | thumbnail_renew | `VmOpc_ThumbnailRenew` (0x432E60) |
| 63 | save_set_font_type | `VmOpc_SaveSetFontType` (0x432800) |
| 64 | set_load_after_process | `VmOpc_SetLoadAfterProcess` (0x431260) |
| 65 | savesystemdata | `VmOpc_SaveSystemData` (0x431220) |
| 66 | save_set_font_effect | `VmOpc_SaveSetFontEffect` (0x4327A0) |
| 67 | save_set_font_color | `VmOpc_SaveSetFontColor` (0x432710) |
| 68 | delete_file | `VmOpc_DeleteFile` (0x431130) |
| 69 | save_tmp_dat | `VmOpc_SaveTmpDat` (0x430F50) |
| 70 | copy_file | `VmOpc_CopyFile` (0x430E10) |
| 71 | load_thumbnail | `VmOpc_LoadThumbnail` (0x432C20) |
| 72 | save_lock_not_open_savefileno | `VmOpc_SaveLockNotOpenSavefileno` (0x432190) |
| 73 | is_save_lock | `VmOpc_IsSaveLock` (0x432330) |
| 74 | is_prev_data | `VmOpc_IsPrevData` (0x430DD0) |
| 75 | save_point_clear | `VmOpc_SavePointClear` (0x432080) |
| 76 | save_point_lock | `VmOpc_SavePointLock` (0x432020) |
| 77 | op_077 | `VmOpc_Op077` (0x433ED0) |
| 78 | histload | `VmOpc_HistLoad` (0x433EF0) |

### Opcode Map (SE opcodes 80-94)

These also serve as extcall category 5 (SE) handler table.
`gExtcallTableCat5_Se` = `gOpcodeDispatchTable[80]` = 0x4BB310.

| # | Name | Handler |
|---|------|---------|
| 80 | se_load | `VmOpc_SeLoad` (0x4353C0) |
| 81 | se_play | `VmOpc_SePlay` (0x435200) |
| 82 | se_play_ex_ch | `VmOpc_SePlayExCh` (0x434EC0) |
| 83 | se_stop | `VmOpc_SeStop` (0x434DB0) |
| 84 | se_set_volume | `VmOpc_SeSetVolume` (0x434C50) |
| 85 | se_get_volume | `VmOpc_SeGetVolume` (0x434BD0) |
| 86 | se_play_ch | `VmOpc_SePlayCh` (0x434B20) |
| 87 | se_stop_ch | `VmOpc_SeStopCh` (0x4345F0) |
| 88 | se_play_ch_ex | `VmOpc_SePlayChEx` (0x434980) |
| 89 | se_stop_all | `VmOpc_SeStopAll` (0x4348F0) |
| 90 | se_play_loop | `VmOpc_SePlayLoop` (0x434820) |
| 91 | se_stop_loop | `VmOpc_SeStopLoop` (0x434750) |
| 92 | se_play_loop_ex | `VmOpc_SePlayLoopEx` (0x434690) |
| 93 | se_stop_loop_ex | `VmOpc_SeStopLoopEx` (0x434540) |
| 94 | se_play_ex | `VmOpc_SePlayEx` (0x434D00) |

### Opcode Map (select opcodes 96-106)

Corrected on 2026-10-23 from table 0x4BB350 and function bodies. Old IDB names
from 0x435C90 downward were shifted; do not infer semantics from those names.
See [Select state machine](softpal-select.md) and evidence/select-voice.json.

| Primary | Category 6 index | Verified semantic name | Handler |
|---|---|---|---|
| 96 | 0 | select_init | 0x4369F0 |
| 97 | 1 | select | 0x436240 |
| 98 | 2 | select_set (inline declaration; standalone error) | 0x4361D0 |
| 99 | 3 | select_commit | 0x435C90 |
| 100 | 4 | select_clear | 0x435C30 |
| 101 | 5 | select_set_offset | 0x435BA0 |
| 102 | 6 | select_set_process | 0x435A70 |
| 103 | 7 | select_lock | 0x435940 |
| 104 | 8 | get_select_on_key | 0x435A30 |
| 105 | 9 | get_select_pull_key | 0x4359F0 |
| 106 | 10 | get_select_push_key | 0x4359A0 |

### Opcode Map (system/font/skip opcodes 108-131)

These also serve as extcall category 9 (system) handler table.
`gExtcallTableCat9_System` = `gOpcodeDispatchTable[108]` = 0x4BB380.

| # | Name | Handler |
|---|------|---------|
| 108 | skip_set | `VmOpc_SkipSet` (0x438C40) |
| 109 | skip_is | `VmOpc_SkipIs` (0x438C00) |
| 110 | auto_set | `VmOpc_AutoSet` (0x438A90) |
| 111 | auto_is | `VmOpc_AutoIs` (0x438A50) |
| 112 | op_112 | `VmOpc_Op112` (0x4389F0) |
| 113 | auto_get_time | `VmOpc_AutoGetTime` (0x4389B0) |
| 114-122 | op_114..op_122 | unidentified |
| 123 | load_font | `VmOpc_LoadFont` (0x4383B0) |
| 124 | unload_font | `VmOpc_UnloadFont` (0x438390) |
| 125 | set_language | `VmOpc_SetLanguage` (0x438090) |
| 126 | key_canncel | `VmOpc_KeyCanncel` (0x437E70) |
| 127 | set_font_color | `VmOpc_SetFontColor` (0x437F30) |
| 128 | load_font_ex | `VmOpc_LoadFontEx` (0x4382D0) |
| 129-131 | op_129..op_131 | unidentified |

## Extcall Dispatch (opcode 23)

**Handler**: `VmOpc_Extcall` at `0x42CF50`

Pseudocode:
```c
int VmOpc_Extcall(DWORD *ctx) {
    DWORD pc = ctx[201015];               // PC (absolute byte offset into ctx)
    DWORD script_base = ctx[201013];      // script buffer base address
    DWORD instr1 = *(DWORD*)(script_base + pc);  // [cat:16|idx:16]
    pc += 4;
    ctx[201015] = pc;                      // advance PC
    DWORD instr2 = *(DWORD*)(script_base + pc);  // argument DWORD
    ctx[201015] += 4;                     // advance PC again
    ctx[184629] = instr2;                 // store extcall arg
    ctx[201018] = instr1;                 // store current extcall ID
    DWORD cat = HIWORD(instr1);           // upper 16 bits = category
    DWORD idx = LOWORD(instr1);           // lower 16 bits = index
    DWORD handler_table = gExtcallCategoryTable[cat];
    if (handler_table)
        return (*(PFUNC*)(handler_table + 4*idx))(ctx);
    return 0;
}
```

**Key context field offsets** (as DWORD indices into ScriptContext):
- `ctx[201013]` = script buffer base address (byte offset 804052)
- `ctx[201015]` = program counter (byte offset 804060)
- `ctx[201018]` = current extcall ID packed word (byte offset 804072)
- `ctx[184629]` = extcall argument (byte offset 738516)

## Extcall Category Map

**Table**: `gExtcallCategoryTable` at `0x49C360` (24 entries, categories 0-23)

| Cat | Hex | Handler Table | Domain |
|-----|-----|--------------|--------|
| 0 | 0x00 | null | — |
| 1 | 0x01 | null | — |
| 2 | 0x02 | `gExtcallTableCat2_Text` (0x4BB490) | ADV text/message |
| 3 | 0x03 | `gExtcallTableCat3_Sprite` (0x4BB038) | Sprite operations |
| 4 | 0x04 | `gExtcallTableCat4_Bgm` (0x4BACF8) | Background music |
| 5 | 0x05 | `gExtcallTableCat5_Se` (0x4BB310) = `gOpcodeDispatchTable[80]` | Sound effects |
| 6 | 0x06 | `gExtcallTableCat6_Select` (0x4BB350) = `gOpcodeDispatchTable[96]` | Choice/select |
| 7 | 0x07 | `gExtcallTableCat7_Wait` (0x4BB5C0) | Wait/timing |
| 8 | 0x08 | `gExtcallTableCat8_Btn` (0x4BAD48) | Button/UI |
| 9 | 0x09 | `gExtcallTableCat9_System` (0x4BB380) = `gOpcodeDispatchTable[108]` | System/font/skip |
| 10 | 0x0A | `gExtcallTableCatA_Save` (0x4BB270) = `gOpcodeDispatchTable[40]` | Save/load |
| 11 | 0x0B | `gExtcallTableCatB_Movie` (0x4BB198) | Movie/animated sprite |
| 12 | 0x0C | `gExtcallTableCatC_SysBtn` (0x4BB47C) | System buttons |
| 13 | 0x0D | `gExtcallTableCatD_Voice` (0x4BB550) | Voice/dubbing |
| 14 | 0x0E | `gExtcallTableCatE_History` (0x4BB130) | History/backlog |
| 15 | 0x0F | `gExtcallTableCatF_Unk` (0x4BADE8) | Unknown |
| 16 | 0x10 | `gExtcallTableCat10_EffectSkip` (0x4BACDC) | Effect skip |
| 17 | 0x11 | `gExtcallTableCat11_Action` (0x4BAC48) | Action system |
| 18 | 0x12 | `gExtcallTableCat12_FileIO` (0x4BAE18) | File I/O |
| 19 | 0x13 | `gExtcallTableCat13_Unk` (0x4BAFF0) | Unknown |
| 20 | 0x14 | `gExtcallTableCat14_Random` (0x4BB004) | Random numbers |
| 21 | 0x15 | `gExtcallTableCat15_Thread` (0x4BB1BC) | Thread/coroutine |
| 22 | 0x16 | `gExtcallTableCat16_Run` (0x4BAFE0) | Script run |
| 23 | 0x17 | `gExtcallTableCat17_Message` (0x4BB25C) | Message queue |

Note: Categories 5, 6, 9, 10 handler tables are **aliased** into the primary opcode dispatch table at the corresponding opcode offsets.

## VM Execution Loop

**Function**: `VmScriptRunStep` at `0x44A2F0`
**Calling convention**: `__thiscall` on `ScriptExecFrame*`

```c
BOOL __thiscall VmScriptRunStep(ScriptExecFrame *this) {
    ScriptContext *ctx = (ScriptContext*)(PalTaskGetTaskData(0) + 824);
    // Loop while not blocked / terminated
    do {
        DWORD pc = this->pc;            // this[2]
        DWORD base = this->script_base; // this[0]
        DWORD instr = *(DWORD*)(base + pc);
        this->pc += 4;
        this->cur_instr = instr;        // this[5]
        if (HIWORD(instr) == 1) {
            // Primary opcode dispatch
            gOpcodeDispatchTable[(u16)instr](ctx);
        }
    } while (!blocked && !stepped);
}
```

**ScriptExecFrame fields** (as DWORD offsets from `this`):
- `this[0]` = script buffer base address
- `this[1]` = execution limit / end address
- `this[2]` = program counter (relative to script base)
- `this[5]` = current instruction word
- `this[7]` = running flag (1 = running)
- `this[8]` = single-step flag (0 = continue, 1 = stop after this)
- `this[9]` = blocked/stopped flag
- `this[42]` = pointer to sub-frame or parent context

## ScriptContext Layout (partial)

The `ScriptContext` is at `PalTaskGetTaskData(0) + 824` bytes.

When accessed as `DWORD *ctx`:

| ctx[n] | Byte offset | Field |
|--------|-------------|-------|
| ctx[184629] | 738516 | extcall argument register |
| ctx[201013] | 804052 | script buffer base address |
| ctx[201015] | 804060 | program counter (absolute into buffer) |
| ctx[201018] | 804072 | current extcall packed ID [cat:16|idx:16] |

Additional context fields (from earlier decompile of sp_set handler in prior session):
- Stack top index: ctx[184628] (byte offset 738512)
- Stack values: ctx[177646 + i] (byte offset 710584 + 4*i), up to 64 elements
- Sprite slot table: byte offset 666388, each slot 344 bytes, max 0x81 slots

## VM Blocking Mechanism

Opcode handlers that block (wait, wait_click, etc.) use a **PC rewind** pattern:

```c
// Pattern in VmExtcall_Wait / VmExtcall_WaitClick:
ctx[201021] = 1;          // mark this frame as blocking
if (!timer_expired && !skip_active) {
    ctx[201015] -= 12;    // rewind PC by 12 bytes = 3 DWORDs:
                           //   1 DWORD = opcode 23 instruction
                           //   1 DWORD = extcall packed ID
                           //   1 DWORD = argument
    return 1;              // blocking
}
// else: timer expired or skip active — fall through without rewinding
```

**`VmGetTaskSchedulingState`** (`0x44AC00`) returns a bitmask:
- bit 4 (0x10) = skip mode active (skips text, waits, etc.)
- bit 5 (0x20) = auto-advance active
- bit 3 (0x08) = advance key / mousewheel input detected
- bit 2 (0x04) = confirm key pressed

Wait handlers check `& 0x30` to skip in skip/auto mode.

**Additional ScriptContext fields** discovered from wait handlers:
| ctx[n] | Byte offset | Field |
|--------|-------------|-------|
| ctx[163809] | 655236 | wait start time (PaltimeGetTime return value) |
| ctx[163810] | 655240 | wait duration in ms (0 = not active / expired) |
| ctx[163814] | 655256 | wait skip-able flag |
| ctx[163815] | 655260 | wait once-flag (ensures init only on first call) |
| ctx[201021] | 804084 | blocking flag (1 = blocking this frame) |

## Extcall Handler Index (Named)

### Cat 8 — Btn (0x4BAD48, 39 entries)

| Idx | Handler | Notes |
|-----|---------|-------|
| 0 | `VmExtcall_BtnInit` (0x0040FC60) | PalButtonCreateEx |
| 1 | `VmExtcall_BtnUninit` (0x0040FB20) | PalButtonRelease |
| 3 | `VmExtcall_BtnSet` (0x0040F550) | PalButtonEntryEx |
| 4 | `VmExtcall_BtnHide` (0x0040F290) | |
| 5 | `VmExtcall_BtnShow` (0x0040F1B0) | |
| 6 | `VmExtcall_BtnSetPos` (0x0040F060) | |
| 7 | `VmExtcall_BtnSetRect` (0x0040EE90) | PalSpriteSetRect |
| 8 | `VmExtcall_BtnRelease` (0x0040F360) | PalButtonDelete |
| 9 | `VmExtcall_BtnGetMouse` (0x0040EC10) | PalInputGetMouseX/Y |
| 10 | `VmExtcall_BtnGetMouseState` (0x0040EAA0) | |
| 11 | `VmExtcall_BtnGetKeyOn` (0x0040EA00) | PalInputGetKeyOn |
| 12 | `VmExtcall_BtnGetKeyPull` (0x0040E930) | PalInputGetKeyPull |
| 13 | `VmExtcall_BtnSetToggle` (0x0040E830) | |
| 14 | `VmExtcall_BtnSetKeyMode` (0x0040E700) | |
| 15 | `VmExtcall_BtnEnable` (0x0040E5B0) | PalButtonCtrl |
| 16 | `VmExtcall_BtnSetAlpha` (0x0040E4B0) | |
| 17 | `VmExtcall_BtnGetPush` (0x0040E430) | |
| 18 | `VmExtcall_BtnSetPriority` (0x0040E180) | PalSpriteChangePriority |
| 19 | `VmExtcall_BtnLock` (0x0040E0C0) | |
| 20 | `VmExtcall_BtnUnlock` (0x0040E040) | |
| 21 | `VmExtcall_BtnSetAnim` (0x0040DE40) | |
| 22 | `VmExtcall_BtnSetHit` (0x0040DD90) | PalButtonSetReaction |
| 23 | `VmExtcall_BtnGetOnMouse` (0x0040E3B0) | |
| 24 | `VmExtcall_BtnSetOnMouse` (0x0040DCD0) | |
| 25 | `VmExtcall_BtnGetOffMouse` (0x0040E330) | |
| 26 | `VmExtcall_BtnOnMouseClear` (0x0040DC20) | |
| 27 | `VmExtcall_BtnCreateEx` (0x0040FE20) | PalCopySpriteToSpriteRGB |
| 28 | `VmExtcall_BtnSetAlpha2` (0x004103B0) | |
| 29 | `VmExtcall_BtnSetPosCtrl` (0x00410790) | PalButtonSetPos |
| 30 | `VmExtcall_BtnSetRectEx` (0x004104D0) | |
| 31 | `VmExtcall_BtnSetZOrder` (0x004105B0) | |
| 32 | `VmExtcall_BtnSetScale` (0x004106B0) | |
| 33 | `VmExtcall_BtnSetAngle` (0x00410910) | |
| 34 | `VmExtcall_BtnSetFocus` (0x00410A10) | |
| 35 | `VmExtcall_BtnSetPosMove` (0x00410AD0) | |
| 36 | `VmExtcall_BtnMode` (0x00410BA0) | PalButtonMode |
| 37 | `VmExtcall_BtnSetAnimEx` (0x00410C10) | |
| 38 | `VmExtcall_BtnGetReaction` (0x00410CE0) | PalButtonGetReaction |

### Cat 11 — Movie/MSP (0x4BB198, 8 entries)

| Idx | Handler | Notes |
|-----|---------|-------|
| 0 | `VmExtcall_MoviePlay` (0x0042E650) | PalVideoPlay |
| 1 | `VmExtcall_MspSetLoopSpEp` (0x0042E380) | PalMSpritePlay+SetLoopPoint |
| 2 | `VmExtcall_MspCls` (0x0042E350) | |
| 3 | `VmExtcall_MspWait` (0x0042E240) | PalMSpriteGetState |
| 4 | `VmExtcall_MspLock` (0x0042E1B0) | PalMSpriteLock |
| 5 | `VmExtcall_MspUnlock` (0x0042E120) | PalMSpriteUnlock |
| 6 | `VmExtcall_MspPlay` (0x0042E030) | PalMSpritePlay |
| 7 | `VmExtcall_MspStop` (0x0042DFA0) | PalMSpriteStop |

### Cat 13 — Voice (0x4BB550, 25 entries)

| Idx | Handler | Notes |
|-----|---------|-------|
| 0 | `VmExtcall_VoicePlay` (0x004447B0) | |
| 1 | `VmExtcall_VoiceStop` (0x00444670) | PalSoundStop/StopFade |
| 2 | `VmExtcall_VoiceSetVolume` (0x004445F0) | PalSoundSetVolume |
| 3 | `VmExtcall_VoiceGetVolume` (0x004445A0) | |
| 4 | `VmExtcall_VoicePlayCh` (0x00443FB0) | |
| 5 | `VmExtcall_VoiceSetVolumeCh` (0x00443CB0) | PalSoundSetVolumeChannel |
| 6 | `VmExtcall_VoiceIsEnable` (0x00443C30) | |
| 7 | `VmExtcall_VoicePlayFade` (0x00444190) | |
| 8 | `VmExtcall_VoicePlayEx` (0x004435F0) | PalSoundPlay/PlayFade |
| 9 | `VmExtcall_VoiceStopEx` (0x004434E0) | PalSoundRelease/StopFade |
| 10 | `VmExtcall_VoiceStopFade` (0x004432E0) | |
| 11 | `VmExtcall_VoiceGetExVolume` (0x00443F30) | |
| 12 | `VmExtcall_VoiceSetExVolumeCh` (0x00443DF0) | PalSoundSetVolumeChannel |
| 13 | `VmExtcall_VoiceCheckEnable` (0x00443BC0) | PalTaskGetTaskData |
| 14 | `VmExtcall_VoiceAutopanInit` (0x00443B60) | |
| 15 | `VmExtcall_VoiceAutopanEnable` (0x00443B00) | |
| 16 | `VmExtcall_VoiceAutopanSizeOver` (0x004438D0) | |
| 17 | `VmExtcall_VoiceIsAutopanEnable` (0x00443AC0) | |
| 18 | `VmExtcall_VoiceWait` (0x00443800) | PaltimeGetTime, blocking |
| 19 | `VmExtcall_VoicePause` (0x004431D0) | PalSoundPause |
| 20 | `VmExtcall_BgvMute` (0x00443080) | BGV = background voice |
| 21 | `VmExtcall_SetBgvVolume` (0x00443150) | |
| 22 | `VmExtcall_GetBgvVolume` (0x00443100) | |
| 23 | `VmExtcall_VoiceSetFade` (0x004433A0) | |
| 24 | `VmExtcall_VoiceMute` (0x004441E0) | |

### Cat 14 — History (0x4BB130, 7 entries)

| Idx | Handler | Notes |
|-----|---------|-------|
| 0 | `VmExtcall_HistInit` (0x0042BEB0) | |
| 1 | `VmExtcall_HistBegin` (0x0042BBA0) | PalSpritePaint, PalFontGetColor |
| 2 | `VmExtcall_HistEnd` (0x0042BAA0) | PalSpriteRelease |
| 3 | `VmExtcall_HistScroll` (0x0042BA80) | |
| 4 | `VmExtcall_HistSetPos` (0x0042BA20) | |
| 5 | `VmExtcall_HistGetHeight` (0x0042B9E0) | |
| 6 | `VmExtcall_HistUpdate` (0x0042B7C0) | PalInputGetMouseX, PalSpriteGetRect |

### Cat 20 — Random/Math (0x4BB004, 7 entries)

| Idx | Handler | Notes |
|-----|---------|-------|
| 0 | `VmExtcall_Random` (0x0042D480) | PalRandomEx |
| 1 | `VmExtcall_Abs` (0x0042D410) | |
| 2 | `VmExtcall_Sin` (0x0042D530) | |
| 3 | `VmExtcall_Cos` (0x0042D5E0) | |
| 4 | `VmExtcall_Tan` (0x0042D690) | |
| 5 | `VmExtcall_Atan` (0x0042D740) | |
| 6 | `VmExtcall_Log` (0x0042D7F0) | |

### Cat 21 — Thread (0x4BB1BC, 4 entries)

| Idx | Handler | Notes |
|-----|---------|-------|
| 0 | `VmExtcall_CreateThread` (0x0042E900) | |
| 1 | `VmExtcall_ExitThread` (0x0042E8C0) | |
| 2 | `VmExtcall_SuspendThread` (0x0042E860) | |
| 3 | `VmExtcall_GetThread` (0x0042E9A0) | |

### Cat 22 — Run/Effect (0x4BAFE0, 6 entries)

| Idx | Handler | Notes |
|-----|---------|-------|
| 0 | `VmExtcall_Run` (0x00421FD0) | PalEffectEx |
| 1 | `VmExtcall_RunNoWait` (0x00421F80) | |
| 2 | `VmExtcall_RunStack` (0x00422160) | PalEffectEx |
| 4 | `VmExtcall_FxEffectCls` (0x00422930) | PalReleaseFxEffect |
| 5 | `VmExtcall_SetFxEffect` (0x00422870) | PalSetFxEffect |

## Key Call Chain

```
start (0x451D52)         ← PE entry, calls __scrt_common_main_seh
  WinMain (0x405C30)     ← __stdcall WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
    if (PalInit(v30))    ← 0x40624A: calls PAL_d.dll PalInit (import stub 0x4500BE)
      PalRun()           ← 0x406476: calls PAL_d.dll PalRun (import stub 0x4500EE)
        [PAL engine task dispatch loop]
          VmScriptRunStep (0x44A2F0)  ← per-frame VM tick, __thiscall ScriptExecFrame*
            gOpcodeDispatchTable[opcode](ctx)     ← typed as VmExtcallHandler[132]
              VmOpc_Extcall (0x42CF50)
                gExtcallCategoryTable[cat][idx](ctx)  ← typed as VmExtcallHandler *[24]
      PalUninit()        ← 0x406390: cleanup
```

PAL function import stubs (Game.exe):
- `PalInit`: 0x4500BE → [__imp_PalInit @ 0x4BBB00]
- `PalRun`: 0x4500EE
- `PalEnd`: 0x45014E
- `PalUninit`: 0x45012E

---

## VmTask Initialization and Tick Loop (Sessions 4–5)

### Task Lifecycle

| Address | Name | Signature | Notes |
|---------|------|-----------|-------|
| 0x4050A0 | VmTask_Main | `__thiscall(unsigned int *task_data)` | Loads system.dat, calls VmCtx_Init if missing, runs tick loop |
| 0x44D320 | VmCtx_Init | `int __thiscall(unsigned int *task_data)` | Zero-init ctx, set volume table to 5000, alloc bufs, write system.dat |
| 0x44D070 | VmTask_WriteSystemDat | `int __thiscall(unsigned int *task_data)` | Open SaveFolder\system.dat (CREATE_ALWAYS), write header + volume table |
| 0x445D60 | VmTask_InitScript | `int __thiscall(unsigned int *ctx)` | Load Script.src/File.dat/Text.dat/Mem.dat/Point.dat, alloc save/thumbnail bufs |
| 0x404F50 | VmTask_RunStep | `int __thiscall(unsigned int *ctx)` | Per-tick wrapper: calls VmTask_Tick |
| 0x4457F0 | VmTask_Tick | `int __thiscall(unsigned int *ctx)` | Main per-frame game loop (see below) |
| 0x405380 | VmTask_CreateMain | `int VmTask_CreateMain(void)` | Creates main script task via PalInitParams[19] callback |

### VmTask_Tick Per-Frame Order

1. VmTask_TickEffects (0x445560) — shake/slide/rect effect systems
2. SE/BGM/Voice/ExtVoice auto-release per slot
3. VmAction_TickAll (0x408DF0) → action slots
4. Anim node loop: iterate gAnimNodeRoot chain, step/release dead nodes
5. VmTask_TickSprites (0x4240F0) → sprite system tick
6. VmTask_ApplyMotion (0x4498D0) → commit sprite motion transforms

### VmTask_TickEffects (0x445560)

If ctx[201022] (skip_effects_flag) set, returns immediately. Otherwise dispatches:
- Bit 0 of ctx[201080]: calls VmEffect_TickShake (0x412AC0) — sprite shake
- Bit 1 of ctx[201080]: timed transition effect
- Bit 2 of ctx[201080]: calls VmEffect_TickSlideFade (0x412860) — color fade + slide
- Bit 3 of ctx[201080]: calls VmEffect_TickRectAnim (0x412620) — rect sprite animation

### Animation Node System

Nodes drive per-sprite procedural animation via a bytecode script. Each node runs VmAnim_StepScript each tick.

**Global storage:**

| Address | Name | Description |
|---------|------|-------------|
| 0x4BD7E0 | gAnimNodePool | Static 128-node pool (128 × 84 bytes) |
| 0x4C01E0 | gAnimNodeRoot | Sentinel root node; root.node[20] = first active node |
| 0x4C0208 | gAnimNodeListHead | = gAnimNodeRoot.node[10] (used as array base: `[10]` = root.node[20] = list head) |
| 0x4C0240 | gSpriteMotionTable | Per-sprite motion state table (stride 756 bytes/entry) |
| 0x653398 | gSpriteMotionList | Linked list of sprite motion entries (stride 684+ bytes, next at +684) |
| 0x49C400 | gSineTable | 512-entry DWORD sine/cosine table for effect oscillation |

**Node struct (21 DWORDs = 84 bytes):**

| Field index | Byte | Purpose |
|------------|------|---------|
| [0] | 0 | handle/active flag |
| [1] | 4 | no-commit-transform flag (set 0 by opcode 0x13) |
| [2] | 8 | state: 2=waiting, 3=running, 4=done |
| [3] | 12 | sprite_entry ptr (gSpriteMotionList entry) |
| [4] | 16 | script_buf (animation bytecode) |
| [5] | 20 | script_size |
| [6] | 24 | script_pc (current execution offset) |
| [10] | 40 | callback list head |
| [11..18] | 44..72 | 8 child node ptrs |
| [19] | 76 | prev ptr (doubly-linked list) |
| [20] | 80 | next ptr (doubly-linked list) |

**Key functions:**

| Address | Name | Notes |
|---------|------|-------|
| 0x446510 | VmNode_AllocSlot | Scan pool for free slot (node[0]==0) |
| 0x447440 | VmNode_LoadAni | Load "%s.ani" bytecode file into free slot, insert into active list |
| 0x447040 | VmNode_IsComplete | Step node (VmAnim_StepScript + callbacks); return 0 = dead |
| 0x4471E0 | VmNode_Release | Full teardown: sub-nodes, callbacks, sprite entry, list unlink |
| 0x4464D0 | VmNode_FreeCbList | Free callback list at node[10] only |
| 0x447750 | VmNode_ClearAll | Clear entire gAnimNodePool + gAnimNodeRoot (called on reset) |
| 0x446650 | VmAnim_StepScript | Animation bytecode interpreter (21 opcodes, see below) |
| 0x402E30 | VmAnim_AllocKeyframeCb | Read 36-byte keyframe from script stream, alloc callback |

**VmAnim_StepScript opcodes:**

| Opcode | Type | Description |
|--------|------|-------------|
| 0x00 | scale-interp mode 0 | Scale interpolation (×1.5 target values) |
| 0x01 | timer-wait | Wait until PaltimeGetTime delta >= duration |
| 0x02..0x04 | linear-interp | Timed linear position interpolation (8/8/16-byte params) |
| 0x07 | pos-delta | Accumulate to node[3] position offsets (+376/380/384, +428/432/436) |
| 0x08 | random-timer | Random-duration wait |
| 0x0A | jump | Relative script PC jump |
| 0x0B | child-node | Create child node (clone script buffer) |
| 0x0C | rect-pos | PalSpriteRectSetPos(node[3], x, y) |
| 0x0D | rect-set | PalSpriteSetRect(node[3], &rect) |
| 0x0E | scale-interp mode 1 | Scale interp variant |
| 0x0F | scale-interp mode 2 | Scale interp variant |
| 0x11 | abs-pos-interp | Interpolate to absolute position (relative to current) |
| 0x12 | scale-interp mode 4 | Scale interp variant |
| 0x13 | commit-enable | node[1] = 0 (allow final transform commit) |
| 0x14 | circular-orbit | Circular motion using cos/sin |
| 0x15/0x16 | keyframe | VmAnim_AllocKeyframeCb (36-byte 9-param curve) |
| 0xFC/0xFD | state-check | State machine checks (node[2] 1→2) |
| 0xFF | end | Return 0 (script finished) |

**VmAnimCb callback functions (tick/done pairs):**

| Address | Name | For opcode |
|---------|------|-----------|
| 0x403FD0 | VmAnimCb_TimerTick | 0x01 |
| 0x4033A0 | VmAnimCb_TimerTick2 | 0xFE |
| 0x403BF0 | VmAnimCb_InterpTick_02 | 0x02 |
| 0x403BB0 | VmAnimCb_InterpDone_02 | 0x02 |
| 0x402F30 | VmAnimCb_InterpTick_03 | 0x03 |
| 0x402F00 | VmAnimCb_InterpDone_03 | 0x03 |
| 0x403170 | VmAnimCb_InterpTick_04 | 0x04 |
| 0x4030F0 | VmAnimCb_InterpDone_04 | 0x04 |
| 0x403490 | VmAnimCb_ScaleInterpTick | 0x00/0x0E/0x0F/0x12 |
| 0x403430 | VmAnimCb_ScaleInterpDone | same |
| 0x403DA0 | VmAnimCb_OrbitTick | 0x14 |
| 0x403D40 | VmAnimCb_OrbitDone | 0x14 |
| 0x402C20 | VmAnimCb_KeyframeTick | 0x15/0x16 |
| 0x402B70 | VmAnimCb_KeyframeDone | 0x15/0x16 |

### FxEffect System

Separate from animation nodes. Managed by VmExtcall_FxEffectCreate/Cls/Wait.

| Address | Name | Description |
|---------|------|-------------|
| 0x4BD708 | gFxEffectListHead | Active FxEffect doubly-linked list head |
| 0x4BD710..0x4BD7D0 | gFxEffectPool | FxEffect entry pool (0xC0 bytes) |
| 0x4BD71C..0x4BD7DC | gFxEffectStateBase..g3DEnabled | FxEffect state array |

Entry layout: [4]=next, [5]=&gFxEffectStateBase; old_head[5]=new_entry (doubly-linked).

### History and Script State

| Address | Name | ctx[N] | Notes |
|---------|------|--------|-------|
| 0x44B7B0 | VmHist_PushDataEntry | ctx[201057] | Push 8-byte {data0,data1} to history_list |
| — | ctx[201229..201260] | point_stack[0..31] | 32-entry call-point stack |
| — | ctx[201261] | point_stack_depth | Decremented on pop |
| — | ctx[201083..201088] | shake_* fields | Shake effect control |
| — | ctx[201093..203132] | effect3_slots | 255 sprite slots × 8 DWORDs |
| — | ctx[203141..204537] | effect4_slots | 127 sprite slots × 11 DWORDs |
