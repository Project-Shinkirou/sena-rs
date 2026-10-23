# IDB Changelog

Record of all metadata changes applied to Game.exe and PAL_d.dll IDBs via idalib-mcp.

---

## 2026-06-08 — Session 1 (Game.exe)

### Batch: Rename VM dispatch infrastructure (26 items)

Applied to session `game` (Game.exe.id0/id1/id2/nam/til).

| Address | Old Name | New Name | Type |
|---------|----------|----------|------|
| 0x44A2F0 | sub_44A2F0 | VmScriptRunStep | func |
| 0x42CF50 | sub_42CF50 | VmOpc_Extcall | func |
| 0x4BB1D0 | dword_4BB1D0 | gOpcodeDispatchTable | data |
| 0x49C360 | dword_49C360 | gExtcallCategoryTable | data |
| 0x4BB490 | dword_4BB490 | gExtcallTableCat2_Text | data |
| 0x4BB038 | dword_4BB038 | gExtcallTableCat3_Sprite | data |
| 0x4BACF8 | dword_4BACF8 | gExtcallTableCat4_Bgm | data |
| 0x4BB310 | dword_4BB310 | gExtcallTableCat5_Se | data |
| 0x4BB350 | dword_4BB350 | gExtcallTableCat6_Select | data |
| 0x4BB5C0 | dword_4BB5C0 | gExtcallTableCat7_Wait | data |
| 0x4BAD48 | dword_4BAD48 | gExtcallTableCat8_Btn | data |
| 0x4BB380 | dword_4BB380 | gExtcallTableCat9_System | data |
| 0x4BB270 | dword_4BB270 | gExtcallTableCatA_Save | data |
| 0x4BB198 | dword_4BB198 | gExtcallTableCatB_Movie | data |
| 0x4BB47C | dword_4BB47C | gExtcallTableCatC_SysBtn | data |
| 0x4BB550 | dword_4BB550 | gExtcallTableCatD_Voice | data |
| 0x4BB130 | dword_4BB130 | gExtcallTableCatE_History | data |
| 0x4BADE8 | dword_4BADE8 | gExtcallTableCatF_Unk | data |
| 0x4BACDC | dword_4BACDC | gExtcallTableCat10_EffectSkip | data |
| 0x4BAC48 | dword_4BAC48 | gExtcallTableCat11_Action | data |
| 0x4BAE18 | dword_4BAE18 | gExtcallTableCat12_FileIO | data |
| 0x4BAFF0 | dword_4BAFF0 | gExtcallTableCat13_Unk | data |
| 0x4BB004 | dword_4BB004 | gExtcallTableCat14_Random | data |
| 0x4BB1BC | dword_4BB1BC | gExtcallTableCat15_Thread | data |
| 0x4BAFE0 | dword_4BAFE0 | gExtcallTableCat16_Run | data |
| 0x4BB25C | dword_4BB25C | gExtcallTableCat17_Message | data |

### Batch: Rename primary opcode handlers 1-33 (32 items)

| Address | New Name |
|---------|----------|
| 0x42D3E0 | VmOpc_Mov |
| 0x42D3B0 | VmOpc_Add |
| 0x42D380 | VmOpc_Sub |
| 0x42D350 | VmOpc_Mul |
| 0x42D320 | VmOpc_Div |
| 0x42D260 | VmOpc_Bitand |
| 0x42D230 | VmOpc_Bitor |
| 0x42D200 | VmOpc_Bitxor |
| 0x42D020 | VmOpc_JmpPoint |
| 0x42CFB0 | VmOpc_JfPoint |
| 0x42CDE0 | VmOpc_GosubPoint |
| 0x42D1D0 | VmOpc_Eq |
| 0x42D1A0 | VmOpc_Ne |
| 0x42D170 | VmOpc_Le |
| 0x42D140 | VmOpc_Ge |
| 0x42D110 | VmOpc_Lt |
| 0x42D0E0 | VmOpc_Gt |
| 0x42D0A0 | VmOpc_Lor |
| 0x42D060 | VmOpc_Land |
| 0x42CF10 | VmOpc_LnotSlot |
| 0x42CEB0 | VmOpc_End |
| 0x42CEA0 | VmOpc_Nop |
| 0x42CC20 | VmOpc_Ret |
| 0x42CBF0 | VmOpc_ResetAdv |
| 0x42D2F0 | VmOpc_Mod |
| 0x42D2C0 | VmOpc_Shl |
| 0x42D290 | VmOpc_Shr |
| 0x42CED0 | VmOpc_NegSlot |
| 0x42CBA0 | VmOpc_Pop |
| 0x42CB50 | VmOpc_Push |
| 0x42CAA0 | VmOpc_PackArgs |
| 0x42CA50 | VmOpc_DropArgs |

### Batch: Rename opcode 35-63 handlers (24 items)

| Address | New Name |
|---------|----------|
| 0x42DDD0 | VmOpc_CreateMessage |
| 0x42DD60 | VmOpc_GetMessage |
| 0x42DCF0 | VmOpc_GetMessageParam |
| 0x433D10 | VmOpc_Save |
| 0x433A60 | VmOpc_Load |
| 0x4339B0 | VmOpc_SaveSetTitle |
| 0x433690 | VmOpc_SaveData |
| 0x433350 | VmOpc_SaveSetThumbnailSize |
| 0x432F40 | VmOpc_ThumbnailSet |
| 0x4328C0 | VmOpc_SavetitleDraw |
| 0x432860 | VmOpc_SaveSetFontSize |
| 0x433420 | VmOpc_GetSaveDay |
| 0x432620 | VmOpc_IsSave |
| 0x432480 | VmOpc_GetSaveUserMemory |
| 0x4320C0 | VmOpc_SavePoint |
| 0x432EE0 | VmOpc_SaveThumbnailMosaicSet |
| 0x431C70 | VmOpc_SaveTimeDraw |
| 0x4318C0 | VmOpc_SaveDayDraw |
| 0x431330 | VmOpc_SaveSetTextRect |
| 0x4313C0 | VmOpc_SaveTextDraw |
| 0x4312F0 | VmOpc_GetNewSavefile |
| 0x4338B0 | VmOpc_SetSaveText |
| 0x432E60 | VmOpc_ThumbnailRenew |
| 0x432800 | VmOpc_SaveSetFontType |

### Batch: Rename opcode 64-106 handlers (41 items)

| Address | New Name |
|---------|----------|
| 0x431260 | VmOpc_SetLoadAfterProcess |
| 0x431220 | VmOpc_SaveSystemData |
| 0x4327A0 | VmOpc_SaveSetFontEffect |
| 0x432710 | VmOpc_SaveSetFontColor |
| 0x431130 | VmOpc_DeleteFile |
| 0x430F50 | VmOpc_SaveTmpDat |
| 0x430E10 | VmOpc_CopyFile |
| 0x432C20 | VmOpc_LoadThumbnail |
| 0x432190 | VmOpc_SaveLockNotOpenSavefileno |
| 0x432330 | VmOpc_IsSaveLock |
| 0x430DD0 | VmOpc_IsPrevData |
| 0x432080 | VmOpc_SavePointClear |
| 0x432020 | VmOpc_SavePointLock |
| 0x433ED0 | VmOpc_Op077 |
| 0x433EF0 | VmOpc_HistLoad |
| 0x4353C0 | VmOpc_SeLoad |
| 0x435200 | VmOpc_SePlay |
| 0x434EC0 | VmOpc_SePlayExCh |
| 0x434DB0 | VmOpc_SeStop |
| 0x434C50 | VmOpc_SeSetVolume |
| 0x434BD0 | VmOpc_SeGetVolume |
| 0x434B20 | VmOpc_SePlayCh |
| 0x4345F0 | VmOpc_SeStopCh |
| 0x434980 | VmOpc_SePlayChEx |
| 0x4348F0 | VmOpc_SeStopAll |
| 0x434820 | VmOpc_SePlayLoop |
| 0x434750 | VmOpc_SeStopLoop |
| 0x434690 | VmOpc_SePlayLoopEx |
| 0x434540 | VmOpc_SeStopLoopEx |
| 0x434D00 | VmOpc_SePlayEx |
| 0x4369F0 | VmOpc_SelectInit |
| 0x436240 | VmOpc_SelectWait |
| 0x4361D0 | VmOpc_SelectSet |
| 0x435C90 | VmOpc_SelectClear |
| 0x435C30 | VmOpc_SelectSetOffset |
| 0x435BA0 | VmOpc_SelectSetProcess |
| 0x435A70 | VmOpc_SelectLock |
| 0x435940 | VmOpc_GetSelectOnKey |
| 0x435A30 | VmOpc_GetSelectPullKey |
| 0x4359F0 | VmOpc_GetSelectPushKey |
| 0x4359A0 | VmOpc_GetSelectGetKey |

### Batch: Rename opcode 108-131 handlers (24 items)

| Address | New Name |
|---------|----------|
| 0x438C40 | VmOpc_SkipSet |
| 0x438C00 | VmOpc_SkipIs |
| 0x438A90 | VmOpc_AutoSet |
| 0x438A50 | VmOpc_AutoIs |
| 0x4389F0 | VmOpc_Op112 |
| 0x4389B0 | VmOpc_AutoGetTime |
| 0x438950 | VmOpc_Op114 |
| 0x4388F0 | VmOpc_Op115 |
| 0x438890 | VmOpc_Op116 |
| 0x438860 | VmOpc_Op117 |
| 0x438830 | VmOpc_Op118 |
| 0x438810 | VmOpc_Op119 |
| 0x438770 | VmOpc_Op120 |
| 0x438490 | VmOpc_Op121 |
| 0x438460 | VmOpc_Op122 |
| 0x4383B0 | VmOpc_LoadFont |
| 0x438390 | VmOpc_UnloadFont |
| 0x438090 | VmOpc_SetLanguage |
| 0x437E70 | VmOpc_KeyCanncel |
| 0x437F30 | VmOpc_SetFontColor |
| 0x4382D0 | VmOpc_LoadFontEx |
| 0x437E10 | VmOpc_Op129 |
| 0x437D90 | VmOpc_Op130 |
| 0x437CD0 | VmOpc_Op131 |

### Comments applied (4 items)

| Address | Target |
|---------|--------|
| 0x44A2F0 | VmScriptRunStep function comment |
| 0x42CF50 | VmOpc_Extcall function comment |
| 0x4BB1D0 | gOpcodeDispatchTable data comment |
| 0x49C360 | gExtcallCategoryTable data comment |

### IDB Save (first)

IDB saved after above changes. Total metadata changes: 127 renames + 4 comments.

---

### Batch: Rename wait extcall handlers (11 items)

| Address | New Name |
|---------|----------|
| 0x444F40 | VmExtcall_Wait |
| 0x444DE0 | VmExtcall_WaitClick |
| 0x444C50 | VmExtcall_WaitSyncBegin |
| 0x444B60 | VmExtcall_WaitSyncRelease |
| 0x444B20 | VmExtcall_WaitSyncEnd |
| 0x444AC0 | VmExtcall_WaitSyncStep |
| 0x444A50 | VmExtcall_Wait_Ext6 |
| 0x444C90 | VmExtcall_WaitClickNoAnim |
| 0x444AE0 | VmExtcall_WaitSyncGetTime |
| 0x444A20 | VmExtcall_WaitTimePush |
| 0x4449E0 | VmExtcall_WaitTimePop |

### Batch: Rename sprite extcall handlers (13 items)

| Address | New Name |
|---------|----------|
| 0x4299A0 | VmExtcall_SpSet |
| 0x429350 | VmExtcall_SpSetEx |
| 0x428940 | VmExtcall_SpSetPos |
| 0x428E20 | VmExtcall_SpCls |
| 0x4281D0 | VmExtcall_SpSetAlpha |
| 0x428D20 | VmExtcall_SpClsEx |
| 0x4275F0 | VmExtcall_SetFilter |
| 0x428000 | VmExtcall_SpSetScale |
| 0x426950 | VmExtcall_SpText |
| 0x428640 | VmExtcall_SpSetPosMove |
| 0x425640 | VmExtcall_SpSetShake |
| 0x424E70 | VmExtcall_SpShow |
| 0x424DB0 | VmExtcall_SpHide |

### Batch: Rename BGM extcall handlers (5 items)

| Address | New Name |
|---------|----------|
| 0x40C4B0 | VmExtcall_BgmPlay |
| 0x40C380 | VmExtcall_BgmStop |
| 0x40C2B0 | VmExtcall_BgmSetVolume |
| 0x40C330 | VmExtcall_BgmGetVolume |
| 0x40C8D0 | VmExtcall_BgmLoad |

### Batch: Rename text extcall handlers (14 items)

| Address | New Name |
|---------|----------|
| 0x441400 | VmExtcall_TextInit |
| 0x440B00 | VmExtcall_TextSetIcon |
| 0x440390 | VmExtcall_Text |
| 0x43F340 | VmExtcall_TextHide |
| 0x43F280 | VmExtcall_TextShow |
| 0x43F0B0 | VmExtcall_TextClear |
| 0x43FFC0 | VmExtcall_TextW |
| 0x43FC60 | VmExtcall_TextA |
| 0x43F900 | VmExtcall_TextWA |

### Batch: Rename misc (2 items)

| Address | New Name |
|---------|----------|
| 0x44AC00 | VmGetTaskSchedulingState |

### Comments applied (3 items)

| Address | Target |
|---------|--------|
| 0x444F40 | VmExtcall_Wait — blocking mechanism, PC rewind |
| 0x444DE0 | VmExtcall_WaitClick — blocking mechanism |
| 0x44AC00 | VmGetTaskSchedulingState — bitmask description |

### IDB Save (second)

Saved after wait/sprite/BGM/text handler renames. Total additional: 46 renames + 3 comments.

---

### Batch: Rename Cat8 Btn extcall handlers (38 items)

| Index | Address | New Name |
|-------|---------|----------|
| 0 | 0x0040FC60 | VmExtcall_BtnInit |
| 1 | 0x0040FB20 | VmExtcall_BtnUninit |
| 3 | 0x0040F550 | VmExtcall_BtnSet |
| 4 | 0x0040F290 | VmExtcall_BtnHide |
| 5 | 0x0040F1B0 | VmExtcall_BtnShow |
| 6 | 0x0040F060 | VmExtcall_BtnSetPos |
| 7 | 0x0040EE90 | VmExtcall_BtnSetRect |
| 8 | 0x0040F360 | VmExtcall_BtnRelease |
| 9 | 0x0040EC10 | VmExtcall_BtnGetMouse |
| 10 | 0x0040EAA0 | VmExtcall_BtnGetMouseState |
| 11 | 0x0040EA00 | VmExtcall_BtnGetKeyOn |
| 12 | 0x0040E930 | VmExtcall_BtnGetKeyPull |
| 13 | 0x0040E830 | VmExtcall_BtnSetToggle |
| 14 | 0x0040E700 | VmExtcall_BtnSetKeyMode |
| 15 | 0x0040E5B0 | VmExtcall_BtnEnable |
| 16 | 0x0040E4B0 | VmExtcall_BtnSetAlpha |
| 17 | 0x0040E430 | VmExtcall_BtnGetPush |
| 18 | 0x0040E180 | VmExtcall_BtnSetPriority |
| 19 | 0x0040E0C0 | VmExtcall_BtnLock |
| 20 | 0x0040E040 | VmExtcall_BtnUnlock |
| 21 | 0x0040DE40 | VmExtcall_BtnSetAnim |
| 22 | 0x0040DD90 | VmExtcall_BtnSetHit |
| 23 | 0x0040E3B0 | VmExtcall_BtnGetOnMouse |
| 24 | 0x0040DCD0 | VmExtcall_BtnSetOnMouse |
| 25 | 0x0040E330 | VmExtcall_BtnGetOffMouse |
| 26 | 0x0040DC20 | VmExtcall_BtnOnMouseClear |
| 27 | 0x0040FE20 | VmExtcall_BtnCreateEx |
| 28 | 0x004103B0 | VmExtcall_BtnSetAlpha2 |
| 29 | 0x00410790 | VmExtcall_BtnSetPosCtrl |
| 30 | 0x004104D0 | VmExtcall_BtnSetRectEx |
| 31 | 0x004105B0 | VmExtcall_BtnSetZOrder |
| 32 | 0x004106B0 | VmExtcall_BtnSetScale |
| 33 | 0x00410910 | VmExtcall_BtnSetAngle |
| 34 | 0x00410A10 | VmExtcall_BtnSetFocus |
| 35 | 0x00410AD0 | VmExtcall_BtnSetPosMove |
| 36 | 0x00410BA0 | VmExtcall_BtnMode |
| 37 | 0x00410C10 | VmExtcall_BtnSetAnimEx |
| 38 | 0x00410CE0 | VmExtcall_BtnGetReaction |

### Batch: Rename Cat13 Voice extcall handlers (25 items)

| Index | Address | New Name |
|-------|---------|----------|
| 0 | 0x004447B0 | VmExtcall_VoicePlay |
| 1 | 0x00444670 | VmExtcall_VoiceStop |
| 2 | 0x004445F0 | VmExtcall_VoiceSetVolume |
| 3 | 0x004445A0 | VmExtcall_VoiceGetVolume |
| 4 | 0x00443FB0 | VmExtcall_VoicePlayCh |
| 5 | 0x00443CB0 | VmExtcall_VoiceSetVolumeCh |
| 6 | 0x00443C30 | VmExtcall_VoiceIsEnable |
| 7 | 0x00444190 | VmExtcall_VoicePlayFade |
| 8 | 0x004435F0 | VmExtcall_VoicePlayEx |
| 9 | 0x004434E0 | VmExtcall_VoiceStopEx |
| 10 | 0x004432E0 | VmExtcall_VoiceStopFade |
| 11 | 0x00443F30 | VmExtcall_VoiceGetExVolume |
| 12 | 0x00443DF0 | VmExtcall_VoiceSetExVolumeCh |
| 13 | 0x00443BC0 | VmExtcall_VoiceCheckEnable |
| 14 | 0x00443B60 | VmExtcall_VoiceAutopanInit |
| 15 | 0x00443B00 | VmExtcall_VoiceAutopanEnable |
| 16 | 0x004438D0 | VmExtcall_VoiceAutopanSizeOver |
| 17 | 0x00443AC0 | VmExtcall_VoiceIsAutopanEnable |
| 18 | 0x00443800 | VmExtcall_VoiceWait |
| 19 | 0x004431D0 | VmExtcall_VoicePause |
| 20 | 0x00443080 | VmExtcall_BgvMute |
| 21 | 0x00443150 | VmExtcall_SetBgvVolume |
| 22 | 0x00443100 | VmExtcall_GetBgvVolume |
| 23 | 0x004433A0 | VmExtcall_VoiceSetFade |
| 24 | 0x004441E0 | VmExtcall_VoiceMute |

### Batch: Rename Cat11 Movie/MSP extcall handlers (8 items)

| Index | Address | New Name |
|-------|---------|----------|
| 0 | 0x0042E650 | VmExtcall_MoviePlay |
| 1 | 0x0042E380 | VmExtcall_MspSetLoopSpEp |
| 2 | 0x0042E350 | VmExtcall_MspCls |
| 3 | 0x0042E240 | VmExtcall_MspWait |
| 4 | 0x0042E1B0 | VmExtcall_MspLock |
| 5 | 0x0042E120 | VmExtcall_MspUnlock |
| 6 | 0x0042E030 | VmExtcall_MspPlay |
| 7 | 0x0042DFA0 | VmExtcall_MspStop |

### Batch: Rename Cat14 History extcall handlers (7 items)

| Index | Address | New Name |
|-------|---------|----------|
| 0 | 0x0042BEB0 | VmExtcall_HistInit |
| 1 | 0x0042BBA0 | VmExtcall_HistBegin |
| 2 | 0x0042BAA0 | VmExtcall_HistEnd |
| 3 | 0x0042BA80 | VmExtcall_HistScroll |
| 4 | 0x0042BA20 | VmExtcall_HistSetPos |
| 5 | 0x0042B9E0 | VmExtcall_HistGetHeight |
| 6 | 0x0042B7C0 | VmExtcall_HistUpdate |

### Batch: Rename Cat20 Random/Math extcall handlers (7 items)

| Index | Address | New Name |
|-------|---------|----------|
| 0 | 0x0042D480 | VmExtcall_Random |
| 1 | 0x0042D410 | VmExtcall_Abs |
| 2 | 0x0042D530 | VmExtcall_Sin |
| 3 | 0x0042D5E0 | VmExtcall_Cos |
| 4 | 0x0042D690 | VmExtcall_Tan |
| 5 | 0x0042D740 | VmExtcall_Atan |
| 6 | 0x0042D7F0 | VmExtcall_Log |

### Batch: Rename Cat21 Thread extcall handlers (4 items)

| Index | Address | New Name |
|-------|---------|----------|
| 0 | 0x0042E900 | VmExtcall_CreateThread |
| 1 | 0x0042E8C0 | VmExtcall_ExitThread |
| 2 | 0x0042E860 | VmExtcall_SuspendThread |
| 3 | 0x0042E9A0 | VmExtcall_GetThread |

### Batch: Rename Cat22 Run/Effect extcall handlers (5 items)

| Index | Address | New Name |
|-------|---------|----------|
| 0 | 0x00421FD0 | VmExtcall_Run |
| 1 | 0x00421F80 | VmExtcall_RunNoWait |
| 2 | 0x00422160 | VmExtcall_RunStack |
| 4 | 0x00422930 | VmExtcall_FxEffectCls |
| 5 | 0x00422870 | VmExtcall_SetFxEffect |

### IDB Save (third)

Saved after btn/voice/movie/history/random/thread/run handler renames. Total additional: 94 renames.

---

### Batch: Rename Cat12/15/17/18 extcall handlers (57 items)

Cat12 SysBtn: SysBtnSet, SysBtnRelease, SysBtnEnable (3)
Cat15 Debug: DebugIs, DebugGetWindow, DebugPrint, DebugTaskData, DebugHex, DebugWindowSet, DebugWindowGet, DebugUnk9, DebugBreak (9)
Cat17 Action: ActionRunCountOver, ActionSyncRunCountOver, ActionRun, ActionClearCountOver, ActionSyncWait, ActionTimeline{5-21}, ActionPush, ActionPop, SetActiveAction, GetActiveAction, ActionRunAll, ActionScriptPush, ActionLoopRunAll, ActionUninit, ActionTimeline{29,31,32}, SetActionClear (32)
Cat18 FileIO: AppExec, IsPlayerName, ArgGet, FileExist, EntryFilename, UpdateAccess, OpenFile, ReadFile, SetFilePointer, SzBuf, GetPrivateProfileInt, plus 3 stubs (13)

### Types Applied (session 2)

- `declare_type`: ScriptExecFrame struct (10 fields), VmExtcallHandler typedef
- `set_type` / `type_apply_batch`: 8 key VM functions (VmScriptRunStep, VmOpc_Extcall, VmExtcall_Wait, etc.)
- `type_apply_batch`: 66 primary opcode handlers — signature `int __cdecl f(unsigned int *ctx)`
- `type_apply_batch`: 103 extcall handlers (btn×38, voice×25, movie×8, hist×7, random×7, thread×4, run×5, effect×6, sysbtn×3)
- `type_apply_batch`: dispatch tables — `VmExtcallHandler[132]` for gOpcodeDispatchTable, `VmExtcallHandler *[24]` for gExtcallCategoryTable

### Batch: Cat16/Cat19 renames (6 items)

Cat16 EffectSkip: EffectSkipRun, EffectSkipWait, EffectSkipInit, EffectSkipStep (4)
Cat19 FxEffect: FxEffectWait, FxEffectCreate (2)

### Batch: Cat3 Sprite — additional handlers (56 items)

Indices 7-60 and 69-79 in gExtcallTableCat3_Sprite: SpSetPriority, SpSet2, SpSetCenterOffset, SpEffectRun, SpSetAngle, SpSetPosRect, SpSetRenderMode, SpMoveEx, SpSetPosEx, SpFreeAll, SpGetColor, FaceCls, SpSetBlend, SpGetAlpha, SpSetGray, SpSetPosMove2, SpGetPos, SpGetPos2, SpSetAnim2-4, SpSetRGBShift, SpAnimeClear, SpSetScaleMove, SpGetScale, SpSetViewCtrl, SpSetColorAdj, SpPaint, SpSetAnim, SpLoadWait, SpDraw, SpSetVisClip, SpUnlock, IsSp, SpSetGamma, SpCreateTransition, SpCopyImage, SpTransitionEx, SpSetTransition, SpGetBackbuffer, SpSetMotion, SpSetMotionPos, SpSetParam, SpSetParam2, SpLock; History aliases: HistScroll2, HistPage, HistUnk71, HistSetBg, HistClear, HistSet, HistSetFaceCall; Sprite extras: SpSetEnv, SpSetEnv2, SpSetFontAdj, SpStopAudio

### Types Applied (session 3)

- `type_apply_batch`: 56 sprite handler signatures + 6 effect handler signatures

### Batch: BGM and Text additional handlers (30 renames)

BGM cat4: BgmGetAutoVolume, BgmWait, BgmSetAutoVolume, BgmPause, BgmGetFilename, BgmPlayFade, SetMasterVolume, GetMasterVolume, MuteMasterVolume, BgmMute (10)
Text cat2: TextInitErr, TextUninit, TextSetRect, TextGetTaskData, TextGetTime, TextSetParam, TextVoicePlay, TextSetAnimTime, TextSetSavePoint×2, TextSetColor, TextIsVisible, TextGetInfo, TextEnableVoiceCut, TextIsVoiceCut, TextSetMode, TextTaskFree, TextSpriteLock, TextSetState, TextRedraw (20)

### Types Applied (session 4)

- `type_apply_batch`: 30 BGM + text handler signatures

### WinMain identification and annotation

WinMain found at 0x405C30 (`_WinMain@16`). Already named by IDA's FLIRT signatures. Applied stdcall prototype. Added comment documenting PalInit/PalRun/PalUninit call sequence. Call sites: PalInit at 0x40624A, PalRun at 0x406476, PalUninit at 0x406390/0x4065CB.

### IDB Save (fifth)

Saved after WinMain annotation. Running total game IDB changes: ~500 renames + 8 comments + ~380 type edits + 2 struct/typedef declarations.

---

## 2026-06-08 — Session 2 (Game.exe) — xref-backed structure recovery

Session reopened (IDB session had closed between conversations).

### Batch: Rename key helper functions (14 new renames)

Phase 2 of deep RE pass: identified and renamed helper functions observed during structure-recovery decompiles.

| Address | Old Name | New Name | Notes |
|---------|----------|----------|-------|
| 0x42F850 | sub_42F850 | VmSaveState_AdvStack | Save-state / thumbnail creation ("AdvStackSavePoint") |
| 0x439380 | sub_439380 | VmHistory_DumpList | Dumps history list entries via PalListGetData |
| 0x44AC90 | sub_44AC90 | VmState_ResetScheduling | Clears scheduling/history fields, frees task handle |
| 0x42EE70 | sub_42EE70 | VmScriptState_SaveHistory | Complex history-save function |
| 0x4024D0 | sub_4024D0 | VmSnd_LoadOgg | Loads .ogg sound via PalSoundLoad |
| 0x44B1E0 | sub_44B1E0 | VmStr_GetFilename | Resource ID → filename string (handles inline strings) |
| 0x43B620 | sub_43B620 | VmText_Reset | Resets text subsystem at ctx+4 |
| 0x449220 | sub_449220 | VmSpr_ReleaseSurface | Releases PAL sprite surface handle |
| 0x4233F0 | sub_4233F0 | VmSpr_FreeSlot | Frees script-side sprite slot |
| 0x447710 | sub_447710 | VmThread_ClearAll | Clears thread list (unk_4BD708) |
| 0x4492A0 | sub_4492A0 | VmSpr_GetSlot | Returns pointer to sprite slot in ctx |
| 0x448710 | sub_448710 | VmSpr_LoadFile | Loads sprite from filesystem path |
| 0x408C40 | sub_408C40 | VmAction_Init | Initializes action slot (called with &slot[204567]) |
| 0x408B00 | sub_408B00 | VmAction_Apply | Applies action (called if not skip mode) |

### Batch: Type signatures for 11 renamed helpers

All 11 functions (excluding VmThread_ClearAll, VmSpr_GetSlot, VmSpr_LoadFile) had signatures applied:
- `VmSaveState_AdvStack`: `int __thiscall(unsigned int *ctx)`
- `VmHistory_DumpList`: `int __thiscall(unsigned int *ctx)`
- `VmState_ResetScheduling`: `int __thiscall(unsigned int *ctx)`
- `VmScriptState_SaveHistory`: `int __thiscall(unsigned int *ctx)`
- `VmSnd_LoadOgg`: `int __fastcall(const char *name, int flags, int loop)`
- `VmStr_GetFilename`: `int __fastcall(char *out_buf, int res_id, int flags, int a4)`
- `VmText_Reset`: `int __thiscall(unsigned int *ctx_text_base)`
- `VmSpr_ReleaseSurface`: `void __cdecl(void *surface_ptr)`
- `VmSpr_FreeSlot`: `void __cdecl(unsigned int *slot_ptr)`
- `VmAction_Init`: `int __cdecl(unsigned int *slot_active_ptr)`
- `VmAction_Apply`: `int __cdecl(unsigned int *slot_active_ptr)`

### New documentation created

- `reverse/PARAMETER_PASSING_RE.md` — calling conventions, stack protocol, ctx vs task_data, PAL bridge
- `reverse/ENGINE_CONTEXT_RE.md` — complete ScriptContext field map with byte ranges and evidence
- `reverse/STRUCTS_RE.md` — fully rewritten with field-level evidence tables for all known structs

### IDB Save (sixth)

Saved after initial Session 2 renames and type applications.

### Batch: Sprite animation helpers (2 renames + 1 data label + 2 types)

| Address | Old Name | New Name | Notes |
|---------|----------|----------|-------|
| 0x449120 | sub_449120 | VmSpr_SlotToAnimIdx | Converts script slot idx to internal motion slot idx |
| 0x449190 | sub_449190 | VmSpr_ReleaseHandle | Releases PAL sprite handle |
| 0x4C0240 | unk_4C0240 | gSpriteMotionTable | Global sprite motion data table (756 bytes/entry) |

Types applied: `VmSpr_SlotToAnimIdx(int slot_idx) → int`, `VmSpr_ReleaseHandle(void *handle) → void`.

### IDB Save (seventh)

Saved after sprite motion table additions.
Running total game IDB changes: ~517 renames + 8 comments + ~393 type edits + 2 struct/typedef declarations.

### Batch: Voice/SE helper functions (7 renames + 4 types)

| Address | Old Name | New Name | Notes |
|---------|----------|----------|-------|
| 0x442280 | sub_442280 | VmVoice_CheckWait | Checks voice-play wait (bitmask at ctx[163819]) |
| 0x434140 | sub_434140 | VmSE_CheckWait | Checks SE-play wait (slot# at ctx[163820]) |
| 0x4426A0 | sub_4426A0 | VmVoice_ReleaseChannel | Releases voice slot(s) via PalSoundRelease |
| 0x4423A0 | sub_4423A0 | VmVoice_ResetState | Resets voice subsystem state |
| 0x442C10 | sub_442C10 | VmVoice_StartPlay | Starts voice playback (PalSoundLoad/Play) |
| 0x4340F0 | sub_4340F0 | VmSE_PostLoad | Post-load callback after SE load |
| 0x402A20 | sub_402A20 | VmSnd_ClearHandle | Clears a PAL sound handle |

Types applied: VmVoice_CheckWait, VmSE_CheckWait, VmVoice_ReleaseChannel, VmVoice_StartPlay.

### IDB Save (eighth)

Running total game IDB changes: ~524 renames + 8 comments + ~397 type edits + 2 struct/typedef declarations.

---

## 2026-06-09 — Session 3 (Game.exe)

### Batch: Thread state offset labels (4 data renames)

Resolved the `unk_54CEXX` pattern: in x86 machine code these are 32-bit displacement fields in
`[esi + 0x54CEXX]` instructions where esi = ctx_bytes. IDA auto-labels the constant as a BSS reference.
Actual meaning: byte offset within the ctx buffer at the given value.

| Address | Old Name | New Name | ctx_dword index |
|---------|----------|----------|-----------------|
| 0x54CE2C | unk_54CE2C | ctx_off_thread_active | ctx[1389451] |
| 0x54CE30 | unk_54CE30 | ctx_off_thread_running | ctx[1389452] |
| 0x54CE38 | unk_54CE38 | ctx_off_thread_target_pc | ctx[1389454] |
| 0x54CE3C | unk_54CE3C | ctx_off_thread_id | ctx[1389455] |

Thread subsystem state region: ctx[1,389,451..1,389,455], byte offsets 0x54CE2C..0x54CE3C.
Immediately follows action slot region (action slot 15 ends at ctx[1,389,446]).

### Batch: Thread handler types (4 types applied)

| Address | Name | Signature |
|---------|------|-----------|
| 0x42E900 | VmExtcall_CreateThread | int __cdecl(unsigned int *ctx) |
| 0x42E8C0 | VmExtcall_ExitThread | int __cdecl(unsigned int *ctx) |
| 0x42E860 | VmExtcall_SuspendThread | int __cdecl(unsigned int *ctx) |
| 0x42E9A0 | VmExtcall_GetThread | int __cdecl(unsigned int *ctx) |

### Batch: Phase 4 WinMain / startup renames (9 items)

**Functions:**

| Address | Old Name | New Name | Notes |
|---------|----------|----------|-------|
| 0x4053D0 | sub_4053D0 | WinStartup_InitPaths | Version info check + save folder init |
| 0x447C50 | sub_447C50 | WinStartup_ValidateResource | RESOURCE.dll checksum validation (returns BOOL) |
| 0x44C2F0 | sub_44C2F0 | VmArchive_LoadMain | Load archive.dat (main resource archive) |
| 0x44C560 | sub_44C560 | VmArchive_LoadCN | Load archive_cn.dat (Simplified Chinese locale) |
| 0x44C7C0 | sub_44C7C0 | VmArchive_LoadTC | Load archive_tc.dat (Traditional Chinese locale) |

**Data labels:**

| Address | Old Name | New Name | Notes |
|---------|----------|----------|-------|
| 0x4BD0C8 | byte_4BD0C8 | gGameMetadata | 0x620-byte struct: CompanyName[0], short[260], ProductName[520], OriginalFilename[780], SaveFolder[1040] |
| 0x4BD0C0 | unk_4BD0C0 | gResourceChecksumValid | BOOL: RESOURCE.dll checksum match |
| 0x4BAC40 | dword_4BAC40 | gResourceDllValid | BOOL: RESOURCE.dll valid |
| 0x4BAC44 | dword_4BAC44 | gResolutionMode | Resolution selector: 1=800×600, 2=1280×720, 3=1920×1080, etc. |

### IDB Save (ninth)

Saved after thread state labels, thread handler types, and Phase 4 startup renames.

### Batch: Phase 8 utility functions (8 renames + 3 types)

| Address | Old Name | New Name | Purpose |
|---------|----------|----------|---------|
| 0x44BC90 | sub_44BC90 | WinConfig_WriteDefaults | Write config.dat from PalInitParams defaults |
| 0x44E1D0 | sub_44E1D0 | StrRemoveChars | Remove chars in separator set from input string |
| 0x44E9F0 | sub_44E9F0 | StrExtractToken | Extract Nth delimited field from string |
| 0x44D970 | sub_44D970 | PathGetType | 0=missing, 1=file, 2=directory |
| 0x44D8A0 | sub_44D8A0 | PathCreateDir | Create directory |
| 0x44E370 | sub_44E370 | StrLen_Safe | Safe string length |
| 0x447A40 | sub_447A40 | WinResource_GetFileInfo | Get RESOURCE.dll file version/info |
| 0x44B810 | sub_44B810 | WinSetup_WndProc | Setup window message handler |

Types applied: WinConfig_WriteDefaults, StrRemoveChars, PathGetType.

### IDB Save (tenth)

Saved after Phase 8 utility renames.
Running total game IDB changes: ~545 renames + 8 comments + ~404 type edits + 2 struct/typedef declarations.

---

## 2026-06-09 — Session 3 (PAL_d.dll)

Session opened (fresh open at 02:47 UTC). No metadata changes applied (PAL_d.dll already has debug symbols).

Key findings from decompilation (read-only analysis):

**Block global** (central PAL engine state): VA 0x103E1A30 in PAL_d.dll.
- Block[155] (=+0x26C): User32.dll handle
- Block[243] (=+0x3CC): DirectShow/touch init flag
- Block[270] (=+0x438): default parent task node pointer
- Block[271] (=+0x43C): main task node pointer (PalTaskGetTaskData(0) returns *(Block[271]))
- Block[273] (=+0x444): task pool max count
- Block[274] (=+0x448): task pool allocation cursor

**Task node pool**: base at dword_103E1C8C, stride 44 bytes/node.

**PalInitParams struct** (27 DWORDs = 0x6C bytes, initialized by PalDefaultCreate):
- [0]=hInstance, [1]=hSplashWnd, [2]=flags2
- [3]=lpWindowTitle, [4]=lpClassName, [5]=lpIconId, [6]=lpMenuName
- [7]=pad, [8]=dwStyle(0x2CF0000), [9]=targetFPS(60)
- [10]=x, [11]=y, [12]=width(1920), [13]=height(1080)
- [14]=renderWidth(=width), [15]=renderHeight(=height)
- [16]=limit_a(256), [17]=limit_b(512), [18]=limit_c(2048)
- [19]=callback, [20]=callbackArg, [21]=field21
- [22]=visFlags(bit0=hidden), [23]=sysFlags(0x441B=17435)
- [24]=field24(2), [25]=field25(1), [26]=field26(1)

### Batch: Script task entry and ctx extension (8 renames)

**Functions:**

| Address | Old Name | New Name | Purpose |
|---------|----------|----------|---------|
| 0x405380 | sub_405380 | VmTask_CreateMain | PalInitParams[19] callback: creates main script task |
| 0x4050A0 | sub_4050A0 | VmTask_Main | Main script task entry: loads system.dat, sets up ctx |
| 0x402590 | sub_402590 | VmScript_ParseSection | Parses section from system.dat |

**Data labels (ctx/task_data offset markers):**

| Address | Old Name | New Name | task_data offset | ctx_dword index |
|---------|----------|----------|-----------------|-----------------|
| 0x57D800 | unk_57D800 | ctx_off_script_listhead | 0x57D800 | ctx[1439026] |
| 0x57D804 | unk_57D804 | ctx_off_script_buf2 | 0x57D804 | ctx[1439027] |
| 0x57D808 | unk_57D808 | ctx_off_script_buf1 | 0x57D808 | ctx[1439028] |
| 0x57D80C | unk_57D80C | ctx_off_script_buf2_size | 0x57D80C | ctx[1439029] |
| 0x57D810 | unk_57D810 | ctx_off_script_buf1_size | 0x57D810 | ctx[1439030] |

ctx total size updated: ~5,756,124 bytes from ctx base (~5.5 MB task data buffer).

### IDB Save (eleventh)

Saved after script task renames and ctx extension labels.

### Batch: VmCtx_Init and related (4 renames)

| Address/Name | Old Name | New Name | Notes |
|--------------|----------|----------|-------|
| 0x44D320 | sub_44D320 | VmCtx_Init | Zeros 5MB of ctx, sets initial field values |
| 0x44FC40 | sub_44FC40 | DataDecrypt | XOR+ROL decryption used on access.dat |
| 0x4DC2AC | unk_4DC2AC | ctx_zero_size_const | VA = 5,095,084 bytes; used as memset size in VmCtx_Init |
| VmCtx_Init clear | — | — | memset(ctx[165005], 0, 5095084) → zeros ctx[165005..1438776] |

### IDB Save (twelfth)

Saved after VmCtx_Init and DataDecrypt renames. Session 3 complete.
Running total game IDB changes: ~556 renames + 8 comments + ~404 type edits + 2 struct/typedef declarations.

---

## 2026-06-09 — Session 4 (PAL_d.dll) — struct declarations and function type writeback

Session opened fresh (IDB session had closed between conversations).

### Batch: Declare 4 struct types in pal_d IDB

| Type | IDA ordinal | Size | Evidence functions |
|------|------------|------|--------------------|
| `PalSprite` | 258 | 400 bytes (0x190) | PalSpriteCreate_0, PalSpriteSetPos_0, PalSpriteRelease_0, PalSpriteSetColor_0, PalSpriteSetScale_0, PalSpriteSetRect_0 |
| `PalTaskNode` | 259 | 44 bytes (0x2C) | PalTaskCreate_0, PalTaskGetTaskData_0 |
| `PalSoundSlot` | 260 | 88 bytes (0x58) | PalSoundLoad_0, PalSoundPlay_0, PalSoundRelease_0 |
| `PalInitParams` | 261 | 108 bytes (0x6C) | PalDefaultCreate_0, PalInit_0 |

**PalSprite final declaration** (corrected from initial draft):
- `anim_handle` at +0x1C — released by `sub_1011CEED` in PalSpriteRelease_0 (not generic field1C)
- `color` at +0x3C — set to color arg by PalSpriteSetColor_0 (renamed from `color_key`, PalSpriteCreate_0 writes -1 as sentinel, not transparency)
- `scale` at +0x70 — float, initialized to 1.0f (0x3F800000) by PalSpriteCreate_0, set by PalSpriteSetScale_0 (renamed from `alpha`)

**PalTaskNode confirmed fields** (all 7 PalTaskCreate_0 parameters verified):
- `callback` at +0x08, `callback_arg` at +0x0C, `data_size` at +0x04
- `out_data_ptr` (output): `*out_data_ptr = (DWORD)node->data_ptr`
- `type` at +0x18: 1=child node, 2=sibling node
- `param_a7` at +0x14; `active` byte at +0x1C = 1 when alive
- `sub_node` at +0x28 — nested task node pointer

**PalSoundSlot confirmed fields** (from PalSoundPlay_0 COM vtable evidence):
- `dsound_buf` at +0x00 — IDirectSoundBuffer COM vtable pointer
- `play_state`/`loop_flag` at +0x08/+0x0C — from PalSoundPlay_0 dispatch
- `active` at +0x10 — nonzero → PalSoundRelease before reload
- `status_code` at +0x34 — 6 when loading, 4 when releasing
- `sound_type` at +0x38 — sound category (3=BGM, 4=SE, 5=Voice, etc.)
- `volume` at +0x48 — default 10000
- `play_time` at +0x54 — final field

### Batch: Function signatures applied to pal_d (~30 total)

Key verified signatures:

| Function | Signature |
|----------|-----------|
| `PalSpriteCreate_0` | `struct PalSprite * __cdecl(DWORD surface_handle, int width, int height, int layer)` |
| `PalSpriteSetPos_0` | `void __cdecl(struct PalSprite *sprite, DWORD x, DWORD y, DWORD z)` |
| `PalSpriteRelease_0` | `void __cdecl(struct PalSprite *sprite)` |
| `PalSpriteSetColor_0` | `void __cdecl(struct PalSprite *sprite, DWORD color)` |
| `PalSpriteSetScale_0` | `void __cdecl(struct PalSprite *sprite, float scale)` |
| `PalSpriteSetRect_0` | `void __cdecl(struct PalSprite *sprite, LONG left, LONG top, LONG right, LONG bottom)` |
| `PalTaskCreate_0` | `int __cdecl(FARPROC callback, DWORD callback_arg, DWORD data_size, DWORD *out_data_ptr, DWORD type, struct PalTaskNode *parent, DWORD param_a7)` |
| `PalTaskGetTaskData_0` | `void * __cdecl(struct PalTaskNode *node)` |
| `PalSoundLoad_0` | `int __cdecl(const char *path, DWORD sound_type, DWORD loop_flag)` |
| `PalSoundPlay_0` | `int __cdecl(int handle, DWORD volume, DWORD flags)` |
| `PalSoundRelease_0` | `void __cdecl(int handle)` |
| `PalDefaultCreate_0` | `int __cdecl(struct PalInitParams *params)` |
| `PalInit_0` | `int __cdecl(struct PalInitParams *params)` |

### IDB Saves (pal_d, session 4)

4 saves during session: after struct declarations batch, after sprite function signatures, after task/sound signatures, after final verification.

Total pal_d IDB changes (first session with metadata): 4 struct declarations, ~30 function signatures applied, ~6 renames, ~5 comments.

---

## 2026-06-09 — Session 4 (Game.exe) — PalInitParams and VmExtcallHandler writeback

### Batch: Declare 2 type definitions in game IDB

| Type | Kind | Content |
|------|------|---------|
| `PalInitParams` | struct | 27-DWORD layout, same as pal_d; applied to WinMain params stack frame |
| `VmExtcallHandler` | typedef | `int (__cdecl *VmExtcallHandler)(unsigned int *ctx)` |

Readback verified: `VmOpc_Extcall` decompiles `VmExtcallHandler *v5` after typedef declaration.

### Batch: Update signatures to use declared struct types

| Address | Function | Updated Signature |
|---------|----------|-------------------|
| 0x44BC90 | `WinConfig_WriteDefaults` | `int __fastcall WinConfig_WriteDefaults(LPCSTR config_path, struct PalInitParams *params)` |
| 0x44E9F0 | `StrExtractToken` | `int __fastcall StrExtractToken(char *out_buf, const CHAR *src, int src_len, int field_index, char delim)` |

force_recompile applied to WinConfig_WriteDefaults, VmOpc_Extcall, WinMain, and related callers to flush decompiler cache after renames.

### IDB Saves (game, session 4)

2 saves during session.

Running total game IDB changes: ~556 renames + 8 comments + ~409 type edits + 4 struct/typedef declarations. 14 saves. Last save: 2026-06-09 Session 4.

---

## 2026-06-09 — Session 4 continued (Game.exe) — VmCtx_Init and VmTask_WriteSystemDat

### Batch: Rename + type 2 new functions (2 renames + 2 types + 2 comments)

| Address | Old Name | New Name | Notes |
|---------|----------|----------|-------|
| 0x44D070 | sub_44D070 | VmTask_WriteSystemDat | Writes task_data state to `<SaveFolder>\system.dat`: 2-byte magic header, first 0x330 bytes of task_data, volume table, script buf1/buf2 |
| 0x49C3F0 | unk_49C3F0 | gSystemDatMagic | 2-byte magic value written at start of system.dat |

Signatures applied:
- `VmCtx_Init`: `int __thiscall VmCtx_Init(unsigned int *task_data)`
- `VmTask_WriteSystemDat`: `int __thiscall VmTask_WriteSystemDat(unsigned int *task_data)`

Comments added to both functions.

### New context layout findings from VmCtx_Init

Volume table at task_data[165107..165120] / ctx[164901..164914] (all initialized to 5000):
- ctx[164902] = vol_bgm (PalSoundSetVolume type 3)
- ctx[164903] = vol_default (PalSoundSetVolume type 1, NOT voice)
- ctx[164905] = vol_voice (PalSoundSetVolume type 5)
- ctx[164906] = vol_se (PalSoundSetVolume type 4)
- ctx[164907] = vol_movie (PalSoundSetVolume type 6)

VmCtx_Init opens Script.src/Text.dat/File.dat file headers to compute script buffer sizes.
VmTask_WriteSystemDat opens `gGameMetadata[1040]` (SaveFolder path) + `\system.dat`.

### IDB Save (fifteenth)

Saved after VmCtx_Init/VmTask_WriteSystemDat annotations.
Running total game IDB changes: ~558 renames + 10 comments + ~411 type edits + 4 struct/typedef declarations. 15 saves. Last save: 2026-06-09 Session 4 continued.

---

## 2026-06-09 — Session 4 continued 2 (Game.exe) — VmTask_InitScript and task chain

### Batch: Rename + type 4 new functions + 4 data labels (8 renames + 4 types + 3 comments)

| Address | Old Name | New Name | Notes |
|---------|----------|----------|-------|
| 0x445D60 | sub_445D60 | VmTask_InitScript | Load Script.src/File.dat/Text.dat/Mem.dat/Point.dat/graphic.dat; allocate save/thumbnail bufs; init PalRender |
| 0x44CAB0 | sub_44CAB0 | VmTask_PushAccessRecord | Push 65-byte access.dat record (64-char name + 1-byte flag) onto ctx_off_script_listhead |
| 0x404F50 | sub_404F50 | VmTask_RunStep | Thin wrapper: calls VmTask_Tick(ctx) = PalTaskChangeNext target |
| 0x4457F0 | sub_4457F0 | VmTask_Tick | Per-tick main loop: handle debug input, update BGM/voice, run PalRun tasks |

Data labels:
- `unk_4BD7D8` → `gVoiceFormatDetected` (1 if .ogg/.wav voice files found at startup)
- `unk_4BD7DC` → `g3DEnabled` (= Pal3DIs() result)
- `unk_6533E0` → `gGraphicLoaded` (1 when graphic.dat is loaded)
- `unk_6533E8` → `gGraphicDatExt` (graphic.dat secondary region pointer)

Signatures applied:
- `VmTask_InitScript`: `int __thiscall VmTask_InitScript(unsigned int *ctx)`
- `VmTask_PushAccessRecord`: `int __fastcall VmTask_PushAccessRecord(int task_data, const char *record_str, char flag)`
- `VmTask_RunStep`: `int __cdecl VmTask_RunStep(int task_data)`
- `VmTask_Tick`: `int __thiscall VmTask_Tick(unsigned int *ctx)`

Comments added to VmTask_InitScript, VmTask_Tick, VmTask_RunStep.

### New context layout findings from VmTask_InitScript

Script buffer table at ctx[177637..177644]:
- ctx[177637] = File.dat buffer ptr
- ctx[177638] = Text.dat buffer ptr
- ctx[177639] = Mem.dat buffer ptr
- ctx[177640] = Point.dat buffer ptr (= call_frame_arg_base confirmed)
- ctx[177641..177644] = sizes of each file

Save/thumbnail buffers from VmTask_InitScript:
- ctx[2209] = 2MB save buffer A
- ctx[2212] = 2MB save buffer B
- ctx[2196] = thumbnail pixel buffer (192×108×4 = 82944 bytes)
- ctx[2193/2194] = thumbnail width 192 / height 108
- ctx[2197] = thumbnail buffer size 82944

Script execution fields from VmTask_InitScript:
- ctx[0] = PalRenderInit() result (render context handle)
- ctx[201012] = saveload PalList*
- ctx[201014] = Script.src buffer size (new)
- ctx[201017] = initial PC from script header[8..11]
- Script.src format: magic "PSG0" (0x50534730), header 12 bytes, [8..11] = initial PC

access.dat: 65-byte records (64-char string + 1-byte flag), pushed to ctx_off_script_listhead PalList.

Debug pointer array at ctx[184623..184627]: 5 self-pointers into ctx regions registered via PalDebugSetData.

ENGINE_CONTEXT_RE.md updated with all new fields and corrected region 3 (save state), region 8b (file buffer table), new region 10b (debug pointers), region 13 (script execution).

### IDB Save (sixteenth)

Saved after VmTask_InitScript/Tick renames and annotations.

---

## 2026-06-09 — Session 4 continued 3 (Game.exe) — VmTask tick chain decomposition

### Batch: Rename + type VmTask tick chain (14 renames + 5 types + 4 comments)

| Address | Old Name | New Name | Notes |
|---------|----------|----------|-------|
| 0x445560 | sub_445560 | VmTask_TickEffects | Per-tick effect scheduler: bitmask ctx[201081] drives effect callbacks |
| 0x4240F0 | sub_4240F0 | VmTask_TickSprites | Per-tick sprite animation: cycle 128 slots, update frame/pos/color |
| 0x4498D0 | sub_4498D0 | VmTask_ApplyMotion | Commit accumulated motion transforms from gSpriteMotionList to PalSprite* |
| 0x40B840 | sub_40B840 | VmBgm_TickFade | BGM fade-out tick handler (called when slot[2] bit 0x40020000) |
| 0x442430 | sub_442430 | VmVoice_TickFade | Voice fade tick handler (called when voice slot[23]==2) |
| 0x408DF0 | sub_408DF0 | VmAction_TickAll | Per-tick action slot update (called with &ctx[204550]) |

BSS offset labels (byte displacements into ctx):
- `unk_54CE80` → `ctx_off_adv_msg_active` (ctx[1,389,472])
- `unk_54CE84` → `ctx_off_adv_msg_pc` (ctx[1,389,473])
- `unk_54D1B8` → `ctx_off_dbg_flag_b8` / `ctx_off_dbg_flag_c0` / `ctx_off_dbg_window_mode` (debug mode ctx fields)

Data labels:
- `unk_653398` → `gSpriteMotionList` (head of sprite motion transform linked list, stride 684+ bytes)

Comments added to VmTask_ApplyMotion (motion commit flow), VmTask_TickEffects (bitmask scheduler).

### New ctx fields from VmTask_Tick decomposition

| DWORD index | Purpose |
|-------------|---------|
| ctx[201022] | skip_effects_flag: if nonzero, VmTask_TickEffects returns immediately |
| ctx[201061] | key_push_state: `|= PalInputGetKeyPush()` each tick |
| ctx[201080] | effects_bitmask: outer tracking of active effects (bit 1 = timed transition) |
| ctx[201081] | scheduler_pending_mask: bitmask of effects awaiting completion (1/2/4/8 for each effect type) |
| ctx[201091] | effect2_timer_start (byte: v3+804364) |
| ctx[201092] | effect2_timer_duration (byte: v3+804368) |
| ctx[165003] | bgm_auto_vol_flag: if nonzero, auto-adjust BGM volume each tick |
| ctx[165371] | voice_sched_flag: controls voice fade scheduling |
| ctx[1389472] | adv_msg_active: if nonzero with adv_msg_pc, triggers AdvMessageProc PC jump |
| ctx[1389473] | adv_msg_pc: target PC for advanced message auto-advance |

VmTask_Tick flow summary:
1. Debug input (key & 8): toggle window mode at ctx_off_dbg_window_mode/c0/b8
2. AdvMessageProc: if adv_msg_active && adv_msg_pc → jump PC, call VmScriptRunStep
3. IsMovie() check: skip game logic if movie playing
4. VmTask_TickEffects(ctx)
5. SE slots (ctx[163822], 16×24): auto-release if not playing
6. BGM slots (ctx[164206], 2×31): auto-release or tick fade
7. Voice slots (ctx[164268], 8×25): auto-release or tick fade; if bgm_auto_vol_flag → update BGM volume
8. ExtVoice slots (ctx[164493], 8×31): auto-release if done
9. VmAction_TickAll(&ctx[204550])
10. Clear and update linked movie/asset nodes at dword_4C0208[10]
11. VmTask_TickSprites(&ctx[166597])
12. VmTask_ApplyMotion() → commits transforms for all gSpriteMotionList entries

gSpriteMotionList: linked list, stride 684+ bytes, next ptr at +684, transform accumulators cleared at +156 (144 bytes) each tick before accumulation.

### IDB Save (seventeenth)

Saved after VmTask tick chain decomposition.
Running total game IDB changes: ~581 renames + 15 comments + ~419 type edits + 4 struct/typedef declarations. 17 saves. Last save: 2026-06-09 Session 4 continued.

---

## 2026-06-09 — Session 5 (Game.exe)

### Batch: VmNode, VmHist, VmEffect functions + gSineTable (9 items)

Applied to session `game`.

| Address | Old Name | New Name | Type |
|---------|----------|----------|------|
| 0x447040 | sub_447040 | VmNode_IsComplete | func |
| 0x44B7B0 | sub_44B7B0 | VmHist_PushDataEntry | func |
| 0x4471E0 | sub_4471E0 | VmNode_Release | func |
| 0x4464D0 | sub_4464D0 | VmNode_FreeCbList | func |
| 0x412AC0 | sub_412AC0 | VmEffect_TickShake | func |
| 0x412860 | sub_412860 | VmEffect_TickSlideFade | func |
| 0x412620 | sub_412620 | VmEffect_TickRectAnim | func |
| 0x49C400 | dword_49C400 | gSineTable | data |

Signatures applied:
- `int __thiscall VmNode_IsComplete(void *this_node)` — checks node[0] (handle), node[2]==4 (complete) or (node[2]==3 and sub_446650()). Iterates callback list at node[10], removes entries returning 0.
- `void __thiscall VmNode_Release(void *this_node)` — full node teardown: free 8 sub-nodes at this[11..18], run/free callback list, free this[4] (data buf), clear sprite motion fields at this[3]+{8,168..296}, apply final transform deltas if !this[1], memset accumulator block at this[3]+376 (0x4C bytes), unlink from doubly-linked list (prev=this[19]+80, next=this[20]+76), memset this (0x54 bytes).
- `int __thiscall VmNode_FreeCbList(void *this_node)` — iterates this[10] callback list only; calls destructor then frees header+data.
- `int __fastcall VmHist_PushDataEntry(int ctx_bytes, int priority, int data0, int data1)` — allocs 8-byte {data0,data1} record, pushes to ctx[201057] history_list via PalListPush(list, entry, priority).
- `int __thiscall VmEffect_TickShake(int *ctx)` — sprite shake: per-frame random offset ±amplitude at sprite+180/184. ctx[201083]=start_time, ctx[201084]=duration, ctx[201085]=axis(0=both/1=x/2=y), ctx[201086]=amplitude, ctx[201088]=flags.
- `int __fastcall VmEffect_TickSlideFade(unsigned int ctx_bytes)` — type-3 effect: fades sprite alpha (PalSpriteSetColor) over duration, lerps position with sinusoidal X component (gSineTable). ctx[203133]=status(2=active), ctx[203135]=start_time, ctx[203136]=duration, ctx[203139]=count. 255 sprite slots × 8 DWORDs at ctx[201093].
- `int __fastcall VmEffect_TickRectAnim(unsigned int ctx_bytes)` — type-4 effect: cycles 8-frame PalSpriteRectSetPos + lerp with sine. ctx[204538]=status(2=active), ctx[204539]=start_time, ctx[204540]=duration, ctx[204543]=count. 127 sprite slots × 11 DWORDs at ctx[203141].

New ctx fields (from VmEffect_TickShake decompile):
- ctx[201082] = shake_type (cleared on done; OWORD-zeroed with ctx[201083..201085])
- ctx[201083] = shake_start_time
- ctx[201084] = shake_duration
- ctx[201085] = shake_axis (0=both_random, 1=x_only, 2=y_only, 3=both_separate)
- ctx[201086] = shake_amplitude
- ctx[201087] = (cleared with ctx[201086] via QWORD store)
- ctx[201088] = shake_flags (bit 0=apply to focus sprite ctx[1067], bit 1=random_mode2)
- ctx[1067] = focus_sprite_ptr (shake effect target sprite pointer)

From shake effect point-stack path (bit 0 of ctx[201080] set):
- ctx[201229..201260] = point_stack entries[0..31] (32 DWORDs at base+804916)
- ctx[201261] = point_stack_depth (at base+805044)

New ctx fields (from VmEffect_TickSlideFade, offset base = ctx_bytes + 804372):
- ctx[201093] = effect3_slot_base (start of 255 sprite slots × 8 DWORDs)
- ctx[203133] = effect3_status (2=active fade)
- ctx[203135] = effect3_start_time
- ctx[203136] = effect3_duration
- ctx[203139] = effect3_sprite_count

New ctx fields (from VmEffect_TickRectAnim, offset base = ctx_bytes + 812564):
- ctx[203141] = effect4_slot_base (start of 127 sprite slots × 11 DWORDs)
- ctx[204538] = effect4_status (2=active)
- ctx[204539] = effect4_start_time
- ctx[204540] = effect4_duration
- ctx[204543] = effect4_sprite_count

gSineTable (0x49C400): 512-entry DWORD sine/cosine lookup table, used by effect types 3 and 4 for sinusoidal X position oscillation: `delta_x - gSineTable[frame & 0x1FF] * -50.0f`.

### IDB Save (eighteenth)

Saved after VmNode/VmHist/VmEffect function batch + gSineTable rename. 18 saves. Last save: 2026-06-09 Session 5.

### Batch: VmAnim functions (2 items)

| Address | Old Name | New Name |
|---------|----------|----------|
| 0x446650 | sub_446650 | VmAnim_StepScript |
| 0x402E30 | sub_402E30 | VmAnim_AllocKeyframeCb |

**VmAnim_StepScript** (0x446650): `int __thiscall VmAnim_StepScript(void *this_node)`
Per-node animation bytecode interpreter. node[4]=script_buf, node[5]=script_size, node[6]=pc. Called by VmNode_IsComplete when node[2]==3. Opcode dispatch (byte at buf[pc]):
- 0x00/0x0E/0x0F/0x12: scale-interp with mode_flag 0/1/2/4 — alloc 36-byte param block, multiply values × 1.5, tick via sub_403490/sub_403430
- 0x01/0xFE/0x08: timer-wait callbacks via sub_403FD0 or sub_4033A0
- 0x02/0x03/0x04: timed linear interpolation (8/8/16-byte params), callbacks sub_403BF0/sub_402F30/sub_403170
- 0x07: accumulate pos delta to node[3]+376/380/384 (int) and node[3]+428/432/436 (float)
- 0x0A: relative script jump via call stack
- 0x0B: create child node (PalMemoryAlloc(84) = VmNode struct, clones script via memmove)
- 0x0C: PalSpriteRectSetPos(node[3], x, y) — set rect frame
- 0x0D: PalSpriteSetRect(node[3], &rect)
- 0x11: absolute position interpolation (relative to current node[3]+28/32/36 and floats +108..116)
- 0x13: set node[1]=0 (enable final-transform commit in VmNode_Release)
- 0x14: circular orbit animation (cos/sin from sub_48EDBA → _libm_sse2_cos/sin_precise), callbacks sub_403DA0/sub_403D40
- 0x15/0x16: VmAnim_AllocKeyframeCb (36-byte keyframe from script stream)
- 0xFC/0xFD: state checks (node[2] = 1→2)
- 0xFF (or >0xFC): return 0 (done/end marker)
Scheduling: if (VmGetTaskSchedulingState() & 0x30): if node[2]==3 → flush callbacks → node[2]=2; opcode 0x0A → return 1.

**VmAnim_AllocKeyframeCb** (0x402E30): `void *__thiscall VmAnim_AllocKeyframeCb(void *this_node)`
Reads 36-byte keyframe data from script stream (9 DWORDs: x0/y0/x1/y1/x2/y2/x3/y3/duration), allocs 44-byte param block, allocs 16-byte cb entry. Tick=sub_402C20, Done=sub_402B70. script_pc += 36.

Node layout (21 DWORDs = 84 = 0x54 bytes) now fully confirmed:
- node[0]=handle/active, node[1]=no-transform-commit flag (set 0 by opcode 0x13)
- node[2]=state(2=waiting,3=running,4=done), node[3]=sprite_entry ptr (gSpriteMotionList)
- node[4]=script_buf, node[5]=script_size, node[6]=script_pc
- node[7..9]=? (unused/internal), node[10]=callback_list_head
- node[11..18]=8 child-node ptrs, node[19]=prev_ptr, node[20]=next_ptr

### IDB Save (nineteenth)

Saved after VmAnim_StepScript + VmAnim_AllocKeyframeCb. 19 saves. Last save: 2026-06-09 Session 5.

### Batch: VmAnim callback functions (14 items)

All are `int __cdecl` functions called as `tick_fn(node_ptr, cb_entry_ptr)` or `done_fn(node_ptr, cb_entry_ptr)`. Callback entry format: [0]=data ptr, [1]=tick_fn, [2]=done_fn (optional), [3]=next_ptr.

| Address | Old Name | New Name | Purpose |
|---------|----------|----------|---------|
| 0x403FD0 | sub_403FD0 | VmAnimCb_TimerTick | Timer wait tick: elapsed >= duration → node[2]=2, return 0 |
| 0x4033A0 | sub_4033A0 | VmAnimCb_TimerTick2 | Alternate timer tick (opcode 0xFE) |
| 0x403BF0 | sub_403BF0 | VmAnimCb_InterpTick_02 | Opcode 0x02 linear interp tick |
| 0x403BB0 | sub_403BB0 | VmAnimCb_InterpDone_02 | Opcode 0x02 done: apply node[3]+416 += data[0]/100.0 |
| 0x402F30 | sub_402F30 | VmAnimCb_InterpTick_03 | Opcode 0x03 linear interp tick |
| 0x402F00 | sub_402F00 | VmAnimCb_InterpDone_03 | Opcode 0x03 done callback |
| 0x403170 | sub_403170 | VmAnimCb_InterpTick_04 | Opcode 0x04 linear interp tick |
| 0x4030F0 | sub_4030F0 | VmAnimCb_InterpDone_04 | Opcode 0x04 done callback |
| 0x403490 | sub_403490 | VmAnimCb_ScaleInterpTick | Scale/alpha interp tick (opcodes 0x00/0x0E/0x0F/0x12) |
| 0x403430 | sub_403430 | VmAnimCb_ScaleInterpDone | Scale/alpha interp done |
| 0x403DA0 | sub_403DA0 | VmAnimCb_OrbitTick | Circular orbit tick (opcode 0x14, uses cos/sin result) |
| 0x403D40 | sub_403D40 | VmAnimCb_OrbitDone | Circular orbit done |
| 0x402C20 | sub_402C20 | VmAnimCb_KeyframeTick | 36-byte keyframe tick (9-param curve interpolation) |
| 0x402B70 | sub_402B70 | VmAnimCb_KeyframeDone | 36-byte keyframe done |

### IDB Save (twentieth)

Saved after VmAnim callback batch. 20 saves. Last save: 2026-06-09 Session 5.

### Batch: Animation node pool globals + VmNode_ClearAll (4 items)

| Address | Old Name | New Name | Type |
|---------|----------|----------|------|
| 0x447750 | VmThread_ClearAll | VmNode_ClearAll | func |
| 0x4BD7E0 | (unnamed) | gAnimNodePool | data |
| 0x4C01E0 | unk_4C01E0 | gAnimNodeRoot | data |
| 0x4C0208 | dword_4C0208 | gAnimNodeListHead | data |

**VmNode_ClearAll** (0x447750): corrected name (was wrongly named VmThread_ClearAll in Session 2). Iterates the 128-node static animation pool at `gAnimNodePool` (stride 84 bytes per node). For each active node: frees 8 child nodes (callback-list + data per child), frees main callback list at node[10], frees script buffer at node[4], clears sprite motion entry at node[3] (zero fields at +8/168..296), if !node[1]: applies final transform deltas (accumulator block at +376, 0x4C bytes), unlinks from doubly-linked list (node[19]+80/node[20]+76), memsets node (84 bytes). Finally memsets `gAnimNodeRoot` (0x4C01E0, 84 bytes).

**gAnimNodePool** (0x4BD7E0): Static 128-node animation node pool. Each entry = 84 bytes = 21 DWORDs. Layout: [0]=handle/active, [1]=no-commit-transform flag, [2]=state(2=waiting/3=running/4=done), [3]=sprite_entry ptr, [4]=script_buf, [5]=script_size, [6]=script_pc, [10]=cb_list_head, [11..18]=8 child-node ptrs, [19]=prev ptr, [20]=next ptr.

**gAnimNodeRoot** (0x4C01E0): Sentinel/root node (not in the main pool). node[10] = `gAnimNodeListHead` (0x4C0208) = head of the active animation node linked list.

**gAnimNodeListHead** (0x4C0208): Head pointer for active animation node doubly-linked list. Located at gAnimNodeRoot+40 (= gAnimNodeRoot.node[10]).

VmNode struct layout (21 DWORDs = 84 = 0x54 bytes) fully confirmed by VmNode_ClearAll + VmNode_Release analysis.

### IDB Save (twenty-first)

Saved after gAnimNodePool/Root/ListHead renames. 21 saves. Last save: 2026-06-09 Session 5.

### Batch: VmNode_AllocSlot, VmNode_LoadAni (2 items)

| Address | Old Name | New Name |
|---------|----------|----------|
| 0x446510 | sub_446510 | VmNode_AllocSlot |
| 0x447440 | sub_447440 | VmNode_LoadAni |

**VmNode_AllocSlot** (0x446510): Scans gAnimNodePool for a free slot where node[0]==0. Returns node ptr or NULL. Checks 4 nodes per stride (at gAnimNodePool, +84, +168, +252 offsets with v4 step).

**VmNode_LoadAni** (0x447440): Takes filename (a2=const char*) and opens "%s.ani" via PalFileCreate/ReadFile. Calls VmNode_AllocSlot to find free node; initializes node with script buffer. Inserts into gAnimNodeListHead active doubly-linked list.

VmTask_Tick node loop (confirmed): iterates `*(gAnimNodeRoot+80)` (= gAnimNodeRoot.next chain) via +80 offsets. Each iteration: steps 8 child nodes (VmNode_IsComplete), frees dead children; steps main node (VmNode_IsComplete), calls VmNode_Release on dead main nodes.

### IDB Save (twenty-second)

Saved after VmNode_AllocSlot/LoadAni. 22 saves. Last save: 2026-06-09 Session 5.

### Batch: VmAnim callback allocator functions (9 items)

All are `__thiscall` functions that read from the script stream (node[4]+node[6]) and allocate callback entries.

| Address | Old Name | New Name | References |
|---------|----------|----------|------------|
| 0x402FD0 | sub_402FD0 | VmAnim_AllocInterpCb_03 | InterpTick/Done_03 |
| 0x4032E0 | sub_4032E0 | VmAnim_AllocInterpCb_04 | InterpTick/Done_04 |
| 0x403730 | sub_403730 | VmAnim_AllocScaleInterpCb_a | ScaleInterpTick/Done |
| 0x4037F0 | sub_4037F0 | VmAnim_AllocScaleInterpCb_b | ScaleInterpTick/Done |
| 0x4038B0 | sub_4038B0 | VmAnim_AllocScaleInterpCb_c | ScaleInterpTick/Done |
| 0x403970 | sub_403970 | VmAnim_AllocScaleInterpCb_d | ScaleInterpTick/Done |
| 0x403A90 | sub_403A90 | VmAnim_AllocScaleInterpCb_e | ScaleInterpTick/Done |
| 0x403CA0 | sub_403CA0 | VmAnim_AllocInterpCb_02 | InterpTick/Done_02 |
| 0x4033D0 | sub_4033D0 | VmAnim_AllocTimerCb2 | TimerTick2 |

### IDB Save (twenty-third)

Saved after VmAnim allocator batch. 23 saves. Last save: 2026-06-09 Session 5.

### Batch: FxEffect globals + misc renames (6 items)

| Address | Old Name | New Name | Type |
|---------|----------|----------|------|
| 0x4BD708 | unk_4BD708 | gFxEffectListHead | data |
| 0x4BD710 | unk_4BD710 | gFxEffectPool | data |
| 0x4BD71C | unk_4BD71C | gFxEffectStateBase | data |
| 0x4BD7D0 | unk_4BD7D0 | gFxEffectPoolEnd | data |
| 0x402930 | sub_402930 | VmStr_FindInList | func |

FxEffect system layout (in BSS adjacent to gAnimNodePool):
- gFxEffectListHead (0x4BD708): head of active FxEffect entry doubly-linked list. Entry at v14: [4]=next, [5]=&gFxEffectStateBase, *(prev+20)=new_head.
- gFxEffectPool (0x4BD710..0x4BD7D0): FxEffect entry pool (0xC0 bytes).
- gFxEffectStateBase (0x4BD71C..0x4BD7DC): FxEffect state/slot array (up to g3DEnabled).
- g3DEnabled (0x4BD7DC): FxEffect state array end sentinel / 3D mode flag.
- gAnimNodePool (0x4BD7E0): begins immediately after.

Evidence: VmExtcall_FxEffectCreate (0x4224A0) and VmExtcall_FxEffectCls (0x422930).

### IDB Save (twenty-fourth)

Saved after FxEffect globals + VmStr_FindInList. 24 saves. Last save: 2026-06-09 Session 5.

---

## Session 5 — Documentation Update (Post-IDB)

### GAME_VM_RE.md — Sessions 4–5 additions (doc only, no IDB changes)

Added new section "VmTask Initialization and Tick Loop":
- VmTask lifecycle table (VmTask_Main/InitScript/RunStep/Tick/WriteSystemDat/VmCtx_Init/VmTask_CreateMain)
- VmTask_Tick per-frame order (7-step sequence)
- VmTask_TickEffects (0x445560) dispatch logic
- Animation Node System: gAnimNodePool/gAnimNodeRoot/gAnimNodeListHead/gSineTable/gSpriteMotionTable globals
- VmNode 21-DWORD struct layout (all fields)
- VmNode key functions table (7 entries, 0x446510..0x447750)
- VmAnim_StepScript 21-opcode table
- VmAnimCb tick/done pair table (14 entries, 0x402B70..0x403FD0)
- FxEffect system globals and entry layout
- History / script state ctx field table (point_stack, shake, effect3/4 slots)

No new IDB metadata changes in this session beyond the 24 saves already logged.

---

## 2026-06-09 — Session 6 (Game.exe)

### Batch 6-A: Struct declarations — PalSprite + SpriteMotionEntry

Declared two new structs in game IDB (ordinals 256, 257):

- `PalSprite` (400 bytes, 34 fields) — transferred from PAL_d.dll debug symbols. Key fields:
  surface_handle(0x0), rect_left-bottom(0x2C-0x38), width(0x54), height(0x58),
  cell_width(0x5C), cell_height(0x60), layer(0x6C), scale(0x70)

- `SpriteMotionEntry` (688 bytes, 172 DWORDs) — Game.exe-specific layered transform
  accumulator wrapping a PalSprite. Key fields:
  - [0] `PalSprite *sprite` — handle passed to all PalSprite* calls
  - [1..3] `_f1/_f2/_f3` pads
  - [4..6] `pos_a_x/y/z`; [7..9] `pos_b_x/y/z` (committed on release)
  - [10..12] `rot_a/rotscale_a_x/y`; [13..15] `rot_b/rotscale_b_x/y` (committed from delta)
  - [16] `has_rect`; [17..20] `rect_a_left/top/right/bottom`
  - [21] `scale_a`; [22] `scale_b` (float, committed from delta)
  - [23..26] `color_alpha/blue/green/red` (bytes); [24..27] `color_r/g/b/a` (shorts)
  - [26] `aspect_flag`
  - [30..32] `float_30/31/32`; [36..38] `float_36/37/38` (floats, committed from delta)
  - [39..41] `pos_c_x/y/z`; [42..44] `pos_d_x/y/z` (cleared on release)
  - [50..52] `rot_e/rotscale_d_x/y` (cleared on release)
  - [94..112] DELTA REGION (bytes 376–451, zeroed on node release):
    anim_delta_pos_x/y/z, anim_delta_rot/sx/sy, anim_delta_rect_*, anim_delta_scale,
    anim_delta_color_r/g/b/a, anim_delta_f107..f112
  - [171] `SpriteMotionEntry *next` — singly-linked list

Evidence: VmNode_Release commit/zero patterns + VmTask_ApplyMotion layer summation.

### Batch 6-B: VmNode struct redeclare — sprite_entry field typed

Redeclared `VmNode` (84 bytes, 21 fields) with:
- field[3]: `void *sprite_entry` → `SpriteMotionEntry *sprite_entry`
All other fields unchanged.

### Batch 6-C: Global type application

- `0x653398 gSpriteMotionList` → `SpriteMotionEntry *`
- `0x4C0240 gSpriteMotionTable` → `SpriteMotionEntry`

### Batch 6-D: Function signature updates

- `0x447440 VmNode_LoadAni` — third parameter `SpriteMotionEntry *sprite_entry`
- `0x4498D0 VmTask_ApplyMotion` — updated; iterator `i` typed as `SpriteMotionEntry *`

### Verification

Re-decompiled and verified:
- `VmNode_Release (0x4471E0)`: all `_DWORD *` raw indexing replaced by named fields:
  `sprite_entry->pos_b_x`, `->anim_delta_pos_x`, `->color_r_d`, `->rot_b`,
  `->anim_delta_color_a`, `memset(&sprite_entry->anim_delta_pos_x, 0, 0x4Cu)`, etc.
- `VmTask_ApplyMotion (0x4498D0)`: `gSpriteMotionList` iterates as `SpriteMotionEntry *i`;
  all PalSprite* call sites show `i->sprite`; all layer sums show named fields.
- `VmNode_LoadAni (0x447440)`: `v7->sprite_entry = v13` (SpriteMotionEntry*) visible.

No placeholder `_DWORD *` or `void *` remain for sprite_entry accesses.

### Save #25 — 2026-06-09

---

## 2026-06-09 — Session 7 (Game.exe) — Phase 2: ScriptContext *ctx mass application

### Batch 7-A: ScriptContext typedef declaration

Declared `typedef unsigned int ScriptContext;` in Game.exe type library.

Rationale: ScriptContext is ~5.56 MB of task data; cannot be declared as a struct. The typedef
is a cosmetic rename so `ctx[N]` accesses remain valid while parameter names become meaningful.

### Batch 7-B: ScriptContext *ctx — 114 functions (Batches 1+2+3 from session)

Applied `ScriptContext *ctx` first parameter to all of the following (replacing `unsigned int *ctx`,
`_DWORD *a1`, or `int a1` as appropriate). All confirmed via ctx field access patterns
(ctx[184628] = offset 738512, ctx[177646..] = stack, etc.).

**Effects (VmEffect_*):**
- `0x412620 VmEffect_TickRectAnim` __thiscall
- `0x412860 VmEffect_TickSlideFade` __thiscall
- `0x412AC0 VmEffect_TickShake` __thiscall

**Tasks/State (VmTask_*, VmState_*, VmScriptState_*, VmSaveState_*):**
- `0x404F50 VmTask_RunStep` __thiscall
- `0x42F52E VmScriptState_SaveHistory` __thiscall
- `0x42F850 VmSaveState_AdvStack` __thiscall
- `0x44AD32 VmState_ResetScheduling` __thiscall
- `0x44A4B3 VmScriptRunStep` __thiscall
- `0x4461A3 VmTask_InitScript` __thiscall
- `0x4498D0 VmTask_ApplyMotion` (SpriteMotionEntry *ctx promotion)
- `0x447440 VmNode_LoadAni`: `ScriptContext *ctx, const char *name, SpriteMotionEntry *sprite_entry`

**SE (VmSE_*, VmOpc_Se*):**
- `0x434140 VmSE_CheckWait`
- `0x43520E VmOpc_SePlay`

**Text (VmText_*):**
- `0x43B620 VmText_Reset`

**Voice (VmVoice_*):**
- `0x442280 VmVoice_CheckWait`

**Opcode primary handlers (VmOpc_*):**
- `0x42CA50 VmOpc_DropArgs`, `0x42CB50 VmOpc_Push`, `0x42CBF0 VmOpc_ResetAdv`, `0x42CC20 VmOpc_Ret`
- `0x433AC6 VmOpc_Load`, `0x433F92 VmOpc_HistLoad`
- `0x4345F9 VmOpc_SeStopCh`, `0x434ED6 VmOpc_SePlayExCh`
- `0x42D3E0 VmOpc_Mov`, `0x42D3B0 VmOpc_Add`, `0x42D380 VmOpc_Sub`, `0x42D350 VmOpc_Mul`
- `0x42D320 VmOpc_Div`, `0x42D260 VmOpc_Bitand`, `0x42D230 VmOpc_Bitor`, `0x42D200 VmOpc_Bitxor`
- `0x42D020 VmOpc_JmpPoint`, `0x42CFB0 VmOpc_JfPoint`, `0x42CDE0 VmOpc_GosubPoint`
- `0x42D1D0 VmOpc_Eq`, `0x42D1A0 VmOpc_Ne`, `0x42D170 VmOpc_Le`, `0x42D140 VmOpc_Ge`
- `0x42D110 VmOpc_Lt`, `0x42D0E0 VmOpc_Gt`, `0x42D0A0 VmOpc_Lor`, `0x42D060 VmOpc_Land`
- `0x42CF10 VmOpc_LnotSlot`, `0x42CEB0 VmOpc_End`, `0x42CEA0 VmOpc_Nop`, `0x42CF50 VmOpc_Extcall`

**Extcall — BGM (VmExtcall_Bgm*):**
- `0x40C208 VmExtcall_BgmWait`, `0x40C230 VmExtcall_BgmMute`, `0x40C6FC VmExtcall_BgmPlay`
- `0x40C7A0 VmExtcall_BgmPlayFade`, `0x40C8D0 VmExtcall_BgmLoad` (alt), `0x40CAAA VmExtcall_BgmLoad`
- `0x40BC70 VmExtcall_MuteMasterVolume`, `0x40BCF0 VmExtcall_GetMasterVolume`, `0x40BD40 VmExtcall_SetMasterVolume`

**Extcall — Btn (VmExtcall_Btn*):**
- `0x40E5B0 VmExtcall_BtnEnable`, `0x40E700 VmExtcall_BtnSetKeyMode`
- `0x40E830 VmExtcall_BtnSetToggle`, `0x40E930 VmExtcall_BtnGetKeyPull`
- `0x40EA00 VmExtcall_BtnGetKeyOn`, `0x40EAA0 VmExtcall_BtnGetMouseState`
- `0x40F75A VmExtcall_BtnSet`, `0x40FD95 VmExtcall_BtnInit`, `0x4100D3 VmExtcall_BtnCreateEx`

**Extcall — FxEffect:**
- `0x411D50 VmExtcall_EffectSkipStep`, `0x4120C0 VmExtcall_EffectSkipInit`, `0x4122A0 VmExtcall_EffectSkipWait`
- `0x422870 VmExtcall_SetFxEffect`, `0x422930 VmExtcall_FxEffectCls`, `0x422986 VmExtcall_FxEffectCls` (dup)
- `0x42C22D VmExtcall_SpSetFontAdj`

**Extcall — Sprite (VmExtcall_Sp*):**
- `0x424DB0 VmExtcall_SpHide`, `0x424E70 VmExtcall_SpShow`, `0x424F30 VmExtcall_SpUnlock`
- `0x424FD0 VmExtcall_SpSetVisClip`, `0x425060 VmExtcall_SpDraw`, `0x425140 VmExtcall_SpLoadWait`
- `0x4254F0 VmExtcall_SpSetAnim`, `0x425640 VmExtcall_SpSetShake`, `0x4259C0 VmExtcall_SpPaint`
- `0x426410 VmExtcall_SpSetColorAdj`, `0x426950 VmExtcall_SpText`, `0x426E50 VmExtcall_SpFreeAll`
- `0x4273C0 VmExtcall_SpSetPosEx`, `0x427530 VmExtcall_SpSetRenderMode`, `0x427A20 VmExtcall_SpGetColor`
- `0x427E90 VmExtcall_SpMoveEx`, `0x428000 VmExtcall_SpSetScale`, `0x4283B0 VmExtcall_SpSetPosRect`
- `0x4287C0 VmExtcall_SpSetAngle`, `0x428B80 VmExtcall_SpEffectRun`
- `0x4299A0 VmExtcall_SpSet`, `0x429350 VmExtcall_SpSetEx`, `0x428940 VmExtcall_SpSetPos`
- `0x428E20 VmExtcall_SpCls`, `0x4281D0 VmExtcall_SpSetAlpha`, `0x427780 VmExtcall_SpSetPriority`
- `0x4244E0 VmExtcall_SpSet2`, `0x4282B0 VmExtcall_SpSetCenterOffset`
- `0x428D20 VmExtcall_SpClsEx`, `0x4275F0 VmExtcall_SetFilter`

**Extcall — History (VmExtcall_Hist*):**
- `0x42B5F0 VmExtcall_HistClear`, `0x42B650 VmExtcall_HistSetBg`, `0x42B760 VmExtcall_HistUnk71`
- `0x42B780 VmExtcall_HistPage`, `0x42B7A0 VmExtcall_HistScroll2`, `0x42B7C0 VmExtcall_HistUpdate`
- `0x42B9E0 VmExtcall_HistGetHeight`, `0x42BA20 VmExtcall_HistSetPos`
- `0x42BA80 VmExtcall_HistScroll`, `0x42BAA0 VmExtcall_HistEnd`

**Extcall — Msp (VmExtcall_Msp*):**
- `0x42DFA0 VmExtcall_MspStop`, `0x42E030 VmExtcall_MspPlay`, `0x42E120 VmExtcall_MspUnlock`
- `0x42E1B0 VmExtcall_MspLock`, `0x42E240 VmExtcall_MspWait`, `0x42E350 VmExtcall_MspCls`
- `0x42E380 VmExtcall_MspSetLoopSpEp`

**Extcall — Thread (VmExtcall_*Thread):**
- `0x42E860 VmExtcall_SuspendThread`, `0x42E8C0 VmExtcall_ExitThread`, `0x42E900 VmExtcall_CreateThread`

**Extcall — Text (VmExtcall_Text*):**
- `0x43E0F0 VmExtcall_TextRedraw`, `0x43E2E0 VmExtcall_TextSetState`, `0x43E4A0 VmExtcall_TextSpriteLock`
- `0x43E550 VmExtcall_TextTaskFree`, `0x43E590 VmExtcall_TextSetMode`, `0x43F050 VmExtcall_TextGetTaskData`
- `0x43F0B0 VmExtcall_TextClear`, `0x43F1A0 VmExtcall_TextInitErr`, `0x43F280 VmExtcall_TextShow`
- `0x43F340 VmExtcall_TextHide`, `0x440390 VmExtcall_Text`, `0x440920 VmExtcall_TextSetRect`
- `0x440B00 VmExtcall_TextSetIcon`, `0x4411D0 VmExtcall_TextUninit`, `0x441400 VmExtcall_TextInit`
- `0x43EEC3 VmExtcall_TextVoicePlay`

**Extcall — Voice (VmExtcall_Voice*):**
- `0x4432E0 VmExtcall_VoiceStopFade`, `0x4434E0 VmExtcall_VoiceStopEx`, `0x4435F0 VmExtcall_VoicePlayEx`
- `0x443B00 VmExtcall_VoiceAutopanEnable`, `0x443B60 VmExtcall_VoiceAutopanInit`
- `0x443BC0 VmExtcall_VoiceCheckEnable`, `0x443C30 VmExtcall_VoiceIsEnable`
- `0x443DF0 VmExtcall_VoiceSetExVolumeCh`, `0x443F30 VmExtcall_VoiceGetExVolume`
- `0x444190 VmExtcall_VoicePlayFade`

**Extcall — Wait (VmExtcall_Wait*):**
- `0x444A20 VmExtcall_WaitTimePush`, `0x444A50 VmExtcall_Wait_Ext6`
- `0x444AE0 VmExtcall_WaitSyncGetTime`, `0x444C90 VmExtcall_WaitClickNoAnim`

**Extcall — Action (Cat17):**
- `0x4095C0 VmExtcall_SetActiveAction`, `0x40AEB0 VmExtcall_ActionSyncWait`
- `0x40B1E0 VmExtcall_ActionScriptPush`, `0x40B4E0 VmExtcall_ActionRun`
- `0x40B5C0 VmExtcall_ActionSyncRunCountOver`, `0x40B3A0 VmExtcall_SetActionClear`
- `0x40A290 VmExtcall_ActionTimeline10`, `0x40A090 VmExtcall_ActionTimeline11`
- `0x409EA0 VmExtcall_ActionTimeline16`, `0x40A650 VmExtcall_ActionTimeline17`
- `0x409F90 VmExtcall_ActionTimeline20`, `0x40A460 VmExtcall_ActionTimeline32`
- `0x409BC0 sub_409BC0`, `0x409AF0 sub_409AF0`, `0x4097A0 sub_4097A0`

**Extcall — Run (VmExtcall_Run):**
- `0x421F80 VmExtcall_RunNoWait` (moved to Batch 7-D)

### Batch 7-C: ScriptContext *ctx — 67 additional functions

Second sweep of all dispatch tables found 67 remaining functions with untyped ctx.
Applied `int __cdecl Xxx(ScriptContext *ctx)` to all. 67 ok, 0 fail.

**BGM:**
- `0x40BE30 VmExtcall_BgmPause`, `0x40BF70 VmExtcall_BgmGetAutoVolume`
- `0x40BFB0 VmExtcall_BgmSetAutoVolume`, `0x40C2B0 VmExtcall_BgmSetVolume`

**Btn:**
- `0x40DC20 VmExtcall_BtnOnMouseClear`, `0x40DCD0 VmExtcall_BtnSetOnMouse`
- `0x40DE40 VmExtcall_BtnSetAnim`, `0x40E040 VmExtcall_BtnUnlock`
- `0x40E330 VmExtcall_BtnGetOffMouse`, `0x40E3B0 VmExtcall_BtnGetOnMouse`
- `0x40EC10 VmExtcall_BtnGetMouse`, `0x40EE90 VmExtcall_BtnSetRect`
- `0x40F060 VmExtcall_BtnSetPos`, `0x40F360 VmExtcall_BtnRelease`
- `0x40FB20 VmExtcall_BtnUninit`, `0x4103B0 VmExtcall_BtnSetAlpha2`
- `0x410790 VmExtcall_BtnSetPosCtrl`

**FxEffect/Run:**
- `0x421F80 VmExtcall_RunNoWait`, `0x422790 VmExtcall_FxEffectWait`

**Sprite:**
- `0x424620 VmExtcall_SpGetBackbuffer`, `0x424790 VmExtcall_SpTransitionEx`
- `0x424940 VmExtcall_SpCopyImage`, `0x424CE0 VmExtcall_IsSp`
- `0x425870 VmExtcall_SpSetAnim3`, `0x425C00 VmExtcall_SpSetRGBShift`
- `0x425EF0 VmExtcall_SpSetMotion`, `0x426DB0 VmExtcall_FaceCls`
- `0x427810 VmExtcall_SpSetGray`, `0x427940 VmExtcall_SpGetScale`
- `0x4284D0 VmExtcall_SpSetBlend`, `0x428640 VmExtcall_SpSetPosMove`
- `0x428F10 VmExtcall_SpCreateTransition`, `0x42A3A0 VmExtcall_SpSetEnv2`

**History:**
- `0x42BBA0 VmExtcall_HistBegin`, `0x42BE30 VmExtcall_HistSet`
- `0x42BEB0 VmExtcall_HistInit`, `0x42C300 VmExtcall_HistSetFaceCall`

**Audio/Misc:**
- `0x42C2B0 VmExtcall_SpStopAudio`
- `0x42D2C0 VmOpc_Shl`
- `0x42D410 VmExtcall_Abs`, `0x42D480 VmExtcall_Random`
- `0x42D5E0 VmExtcall_Cos`, `0x42D690 VmExtcall_Tan`
- `0x42E9A0 VmExtcall_GetThread`

**Text:**
- `0x43E750 VmExtcall_TextIsVoiceCut`, `0x43E790 VmExtcall_TextEnableVoiceCut`
- `0x43EF10 VmExtcall_TextSetParam`, `0x43F010 VmExtcall_TextGetTime`
- `0x43F410 VmExtcall_TextSetSavePoint2`, `0x43F650 VmExtcall_TextSetSavePoint`
- `0x43F900 VmExtcall_TextWA`, `0x43FC60 VmExtcall_TextA`, `0x43FFC0 VmExtcall_TextW`

**Voice:**
- `0x443800 VmExtcall_VoiceWait`, `0x4438D0 VmExtcall_VoiceAutopanSizeOver`
- `0x443AC0 VmExtcall_VoiceIsAutopanEnable`, `0x443FB0 VmExtcall_VoicePlayCh`
- `0x444670 VmExtcall_VoiceStop`, `0x4447B0 VmExtcall_VoicePlay`

**Wait:**
- `0x444AC0 VmExtcall_WaitSyncStep`, `0x444B20 VmExtcall_WaitSyncEnd`
- `0x444B60 VmExtcall_WaitSyncRelease`, `0x444DE0 VmExtcall_WaitClick`

**Opcodes (int a1 confirmed ctx via offset 738512 / ctx[184628]):**
- `0x4369F0 VmOpc_SelectInit` (was `_DWORD *a1`)
- `0x438C40 VmOpc_SkipSet` (was `int a1`, accesses ctx[184628])
- `0x4353C0 VmOpc_SeLoad` (was `int a1`, accesses ctx[184628]+stack)
- `0x433D10 VmOpc_Save` (was `int a1`, accesses ctx+8860)

Note: `VmBgm_TickFade` (internal BGM object tick, not extcall) left as-is — takes BGM object `_DWORD *this`, not ctx.

### Save #26 — 2026-06-09
