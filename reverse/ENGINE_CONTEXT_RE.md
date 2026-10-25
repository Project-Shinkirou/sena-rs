# Engine Context (ScriptContext) Layout

Complete xref-backed field map of the SoftPAL game script context block.

The ScriptContext is the monolithic state buffer for the main game script task:

```
task_data  = PalTaskGetTaskData(0)
ctx        = (unsigned int *)(task_data + 824)
ctx_bytes  = (char *)(task_data + 824)
```

All offsets below are **from ctx** unless otherwise noted. `byte` offsets mean `ctx_bytes + N`; `DWORD index` means `ctx[N]`.

---

## Layout Overview

| Byte range (from ctx) | DWORD range | Region |
|----------------------|-------------|--------|
| 0 – ~44 | 0 – 11 | Current op scratch / text params; ctx[0] = PalRenderInit handle |
| ~48 – ~64 | 12 – 16 | Short flags |
| ~4244 – 4255 | 1061 – 1063 | Text subsystem state |
| ~4272 – ~4308 | 1068 – ~1077 | Text icon slot handles |
| ~8764 – 8832 | 2191 – 2208 | Save state fields |
| ~8800 – ~8848 | 2200 – 2213 | History/effect state + save buffers |
| ~12960 – 12964 | 3240 – 3241 | Effect timer fields |
| ~88332 | 22083 | History gosub target |
| ~655236 – 655260 | 163809 – 163815 | Wait timer state |
| ~655264 – 657132 | 163816 – 164268 | Audio subsystem object (see below) |
| ~655288 – 656648 | 163822 – 164205 | ↳ SE slots (16 × 24 DWORDs = 384 DWORDs) |
| ~656824 – 656948 | 164206 – 164236 | ↳ BGM slot 0 (31 DWORDs) |
| ~656948 – 657072 | 164237 – 164267 | ↳ BGM slot 1 (31 DWORDs) |
| ~657072 – 657872 | 164268 – 164467 | ↳ Voice slots (8 × 25 DWORDs = 200 DWORDs) |
| ~657972 – 658964 | 164493 – 164740 | ↳ ExtVoice slots (8 × 31 DWORDs, same stride as BGM) |
| ~659608 – 659656 | 164902 – 164914 | Audio volume state (init 5000 each) |
| ~661484 | 165371 | voice_sched_flag (voice fade scheduling) |
| ~660012 | 165003 | bgm_auto_vol_flag (auto-adjust BGM volume each tick) |
| ~666388 – 710732 | 166597 – 177683 | Sprite slot table (128 × 344 bytes) |
| ~710548 – 710580 | 177637 – 177644 | Script data file buffer table (4 ptrs + 4 sizes) |
| ~710584 – 710839 | 177646 – 177709 | VM stack (64 elements) |
| ~710840 – ~971019 | 177710 – 242754 | VM variables (65536 × 4 bytes) |
| ~738444 – 738492 | 184611 – 184627 | Debug data pointer array |
| ~738512 – 738519 | 184628 – 184629 | Stack top / extcall arg |
| ~771296 – | 192824 – | Inline string storage (2047 bytes/slot) |
| ~804048 – 804284 | 201012 – 201071 | Script execution state + lists |
| ~804328 – 804372 | 201082 – 201093 | Shake effect control (ctx[201082..201092]) |
| ~804916 – 805048 | 201229 – 201262 | Point stack (32 entries + depth) |
| ~804372 – 812532 | 201093 – 203133 | Effect type 3 sprite slot table (255 × 8 DWORDs) |
| ~812532 – 812564 | 203133 – 203141 | Effect type 3 control fields |
| ~812564 – 818152 | 203141 – 204538 | Effect type 4 sprite slot table (127 × 11 DWORDs) |
| ~818152 – 818200 | 204538 – 204550 | Effect type 4 control fields |
| ~818268 – | 204567 – | Action slot 0 state |
| 296220 bytes/slot, 16 slots | | Action slots 0–15 (sparse) |
| ~5,557,804 – 5,557,824 | 1,389,451 – 1,389,455 | Thread subsystem state |
| ~5,756,104 – 5,756,124 | 1,439,026 – 1,439,030 | Script data pointers (loaded from system.dat) |

---

## Region 1: Scratch / Text Op Parameters (ctx[0–11])

Set by `VmExtcall_TextInit` (0x441400) after popping 8 values from the VM stack.

| DWORD index | Purpose |
|-------------|---------|
Text subsystem object starts at `ctx[1]` (passed as `ctx + 1` to `VmText_Render` at 0x439590).
`VmText_Reset` (0x43B620) is called with `ctx + 1` (= `&ctx[1]`).

| DWORD index | byte offset | Purpose |
|-------------|-------------|---------|
| ctx[1] | 4 | text_active: nonzero = text initialized; 0 → text handlers return error |
| ctx[2] | 8 | cleared by VmState_ResetScheduling |
| ctx[4] | 16 | text_init param 0 (last pushed = first pop) |
| ctx[5] | 20 | text_init param 7 (first pushed = last pop) |
| ctx[6] | 24 | text_init x/pos param |
| ctx[7] | 28 | text_init y/pos param |
| ctx[8] | 32 | text_init flags (alignment bits: 0x80000003) |
| ctx[9] | 36 | text_init param |
| ctx[10] | 40 | text_init param |
| ctx[11] | 44 | text_init param |
| LOWORD(ctx[12]) | 48 | init flag: set to -1 by TextInit and VmState_ResetScheduling |
| ctx[1049] | 4196 | text_current_line: current dialog arg / string param |
| ctx[1050] | 4200 | text_mode: 65537 (0x10001) set on text output |
| ctx[1051] | 4204 | text_state: cleared to 0 on text output |
| ctx[1057] | 4228 | text_arg0: first stack arg to latest text call |
| ctx[1058] | 4232 | text_arg1: second stack arg |
| ctx[1059] | 4236 | text_arg2: third stack arg |
| ctx[1061] | 4244 | text_sub_param0: copy of ctx[6] from TextInit |
| ctx[1062] | 4248 | text_sub_param1: copy of ctx[7] from TextInit |
| ctx[1063] | 4252 | text_task_handle: freed by VmState_ResetScheduling |

Evidence: `VmExtcall_Text` (0x440390), `VmExtcall_TextInit` (0x441400), `VmState_ResetScheduling` (0x44AC90).

LOWORD(ctx[12]) = -1: set by VmState_ResetScheduling (0x44AC90) and TextInit.

---

## Region 2: Text Subsystem State (ctx[1–~1077])

| DWORD index | Purpose |
|-------------|---------|
| ctx[1061] | copy of text_init param (ctx[6] → here) |
| ctx[1062] | copy of text_init param (ctx[7] → here) |
| ctx[1063] | task handle for text (freed by VmState_ResetScheduling) |
| ctx[1068 + 2*i] | text icon slot i sprite handle (i=0..N) |
| ctx[1069 + 2*i] | text icon slot i animation handle |

`dword_49C3E0[i]` maps icon slot index to a sprite layer ID.
Evidence: `VmExtcall_TextSetIcon` (0x440B00).

---

## Region 1b: Button Group Layout (ctx_byte[0..] stride 4512)

Button groups begin at `ctx_byte[0]` (same as ctx[0]). Each group is 4512 bytes (1128 DWORDs). Group N at `ctx_bytes + 4512 * N`.

Active group index: `LOWORD(ctx[12])` (byte offset 48). 0xFFFF = no active group.

Within button group N (byte offsets from group base = ctx_bytes + 4512*N):

| Byte offset | DWORD from group | Purpose |
|-------------|-----------------|---------|
| +0 | group[0] | group base (overlaps ctx[1] for group 0 = text_active) |
| +8 | group[2] | mode flag (0=normal, 2=show secondary button) |
| +4184 | group[1046] | text flag checked by skip set |
| +4188 | group[1047] | mode flag checked against task state |
| +4200 | group[1050] | text_mode byte (BYTE0: 0x10001 = 0x01) |
| +4272 | group[1068] | icon_sprite[0] handle ptr |
| +4280 | group[1070] | icon_sprite[1] handle ptr |
| +4328 | group[1082] | skip button active index |
| +13004 | group[3251] | PAL button handle |
| +13020+32*i | group[3255+8*i] | sprite handle array entry i |
| +17460 | group[4365] | sprite index (-1 = not set) |

Note: The text subsystem fields (ctx[1..1082]) all fall within button group 0's range (bytes 0..4511). Text and button subsystems share the same region for group 0.

Evidence: `VmOpc_SkipSet` (0x438C40).

## Region 2b: SysBtn Slot Table (ctx[21290+])

SysBtn groups, 4 DWORDs per group. Group N fields at `ctx[4*N + 21290]`.

| Slot-relative index | Purpose |
|--------------------|---------|
| slot[21290+4N] | enabled flag (→1) |
| slot[21291+4N] | active/visible flag (→1) |
| slot[21292+4N] | disabled flag (→-(v6!=0), 0 or -1) |
| slot[21293+4N] | button callback data |

Also accessed in SysBtnSet:
- ctx[177644]: argument stack count or pointer
- ctx[177640]: argument array base pointer
- These 6 DWORDs below stack (177646) form the call argument array.

Evidence: `VmExtcall_SysBtnSet` (0x439270).

## Region 2c: Select Subsystem State (ctx[165421+])

Initialized by `VmOpc_SelectInit` (0x4369F0):

| DWORD index | Purpose |
|-------------|---------|
| ctx[165421] | select_flags |
| ctx[165422] | select_color0 = arg | 0xFF000000 |
| ctx[165423] | select_color1 = arg | 0xFF000000 |
| ctx[165424] | select_color2 = arg | 0xFF000000 |

Evidence: `VmOpc_SelectInit` (0x4369F0).

---

## Region 3: Save State (ctx[2191–2212])

| DWORD index | byte offset | Purpose |
|-------------|-------------|---------|
| ctx[2192] | 8768 | save block base pointer? |
| ctx[2193] | 8772 | thumbnail_width: 192 (init by VmTask_InitScript) |
| ctx[2194] | 8776 | thumbnail_height: 108 (init by VmTask_InitScript) |
| ctx[2195] | 8780 | save_active: nonzero → create thumbnail; 1 = init default |
| ctx[2196] | 8784 | thumbnail_buf: PalMemoryAlloc(192*108*4 = 82944) pixel buffer |
| ctx[2197] | 8788 | thumbnail_buf_size: 82944 bytes (192 × 108 × 4 ARGB) |
| ctx[2200] | 8800 | saved_pc (for history replay) |
| ctx[2207] | 8828 | effect_active (→1 when effect starts) |
| ctx[2208] | 8832 | mode flag (nonzero → skip thumbnail) |
| ctx[2209] | 8836 | saveA_buf: PalMemoryAlloc(0x200000) = 2MB save buffer A |
| ctx[2210] | 8840 | history counter (zeroed on replay) |
| ctx[2211] | 8844 | mirror of ctx[2210] (saved before clear) |
| ctx[2212] | 8848 | saveB_buf: PalMemoryAlloc(0x200000) = 2MB save buffer B |
| ctx[2213] | 8852 | cleared to 0 on init |

Evidence: `VmSaveState_AdvStack` (0x42F850), `VmScriptRunStep` (0x44A2F0), `VmTask_InitScript` (0x445D60).

---

## Region 4: Effect / Timer State (ctx[3240–3241])

| DWORD index | byte offset | Purpose |
|-------------|-------------|---------|
| ctx[3240] | 12960 | effect start time (PaltimeGetTime()) |
| ctx[3241] | 12964 | effect duration (ms; 100 = 100 ms default) |

Evidence: `VmScriptRunStep` (0x44A2F0) sets these when gosub target is hit.

---

## Region 5: History Gosub (ctx[22083])

| DWORD index | byte offset | Purpose |
|-------------|-------------|---------|
| ctx[22083] | 88332 | history gosub target point ID (nonzero when active) |

When nonzero: VmScriptRunStep pushes the target PC to the history list and jumps to it.
Evidence: `VmScriptRunStep` (0x44A2F0) at 0x44a499.

---

## Region 6: Wait Timer State and Audio Subsystem (ctx[163809–164467])

### Wait timer (ctx[163809–163815]):

| DWORD index | byte offset | Purpose |
|-------------|-------------|---------|
| ctx[163809] | 655236 | wait_start_time (PaltimeGetTime()) |
| ctx[163810] | 655240 | wait_duration_ms (0=not started, -1=forever) |
| ctx[163814] | 655256 | wait_skip_flag (1=skippable in auto/skip mode) |
| ctx[163815] | 655260 | wait_once_flag (0=first call, nonzero=already initialized) |

Blocking mechanism: if timer not expired and skip mode not active, handler decrements PC by 12 (`ctx[201015] -= 12`) to re-execute the extcall on the next tick. Sets `ctx[201021] = 1` while blocking.

Evidence: `VmExtcall_Wait` (0x444F40), comment in function.

### Audio control registers (ctx[163816–163821]):

| DWORD index | Byte offset | Purpose |
|-------------|-------------|---------|
| ctx[163816] | 655264 | audio subsystem base — start of VmVoice_ReleaseChannel object |
| ctx[163817] | 655268 | audio control field 1 |
| ctx[163818] | 655272 | audio control field 2 |
| ctx[163819] | 655276 | voice wait bitmask — each bit = one voice slot awaiting completion |
| ctx[163820] | 655280 | SE wait slot number (1-based; 0=no wait; checked by VmSE_CheckWait) |
| ctx[163821] | 655284 | saved SE pan channel value |

Voice bitmask: bit N set → voice slot N is in play-wait. Cleared when PalSoundPlayIs returns false or skip mode.

Key-pull mode: ctx[3239] (byte 12956) — nonzero = use PalInputGetKeyPull, zero = PalInputGetKeyPush.

Evidence: `VmExtcall_VoicePlay` (0x4447B0), `VmOpc_SePlay` (0x435200), `VmVoice_CheckWait` (0x442280), `VmSE_CheckWait` (0x434140).

### SE slots (ctx[163822–164205]):

16 SE slots, 24 DWORDs (96 bytes) each.

```
SE slot i: ctx[163822 + 24*i]   (i = 0..15)
```

| Slot-relative index | Purpose |
|--------------------|---------|
| slot[0] | PAL sound handle |
| slot[1] | resource ID |
| slot[2] | flags (0x20000000 default; bit4=loop; various playback flags) |
| slot[3] | category/loop param |
| slot[7] | cleared on load |
| slot[8..23] | filename string buffer (64 bytes, if inline string bit set) |

Evidence: `VmOpc_SeLoad` (0x4353C0) accesses `(96 * v4 + ctx_bytes + 655288)`.

---

## Region 7: BGM Slots (ctx[164206–164267])

Stride: 31 DWORDs (124 bytes) per slot. 2 slots max. Base at `ctx[164206]` (byte offset 656824).

```
Slot 0: ctx[164206 .. 164236]
Slot 1: ctx[164237 .. 164267]
Slot N alias: ctx[164206 + 31*N]
```

| Slot-relative index | Purpose |
|--------------------|---------|
| slot[0] | PAL sound handle (from PalSoundLoad) |
| slot[1] | sound resource ID / name |
| slot[2] | playback flags (bit0=?, bit4=loop, bit28=inline string) |
| slot[3] | volume (100 = full volume) |
| slot[8..23] | filename string buffer (64 bytes) |
| slot[24] | loop flag = flags & 0x10 |
| slot[25] | fade-in time |
| slot[26] | fade-out time |
| slot[27] | additional playback param |

When slot 0 (v4==-1 case) is vacated: `memset(ctx+164206, 0, 0x7C)` clears 31 DWORDs.
Evidence: `VmExtcall_BgmPlay` (0x40C4B0).

### Voice slots (ctx[164268–164467]):

8 voice slots, 25 DWORDs (100 bytes) each.

```
Voice slot i: ctx[164268 + 25*i]   (i = 0..7)
```

| Slot-relative index | Purpose |
|--------------------|---------|
| slot[0] | PAL sound handle (nonzero = slot in use) |
| slot[1..24] | voice slot data |

`VmSpr_FreeSlot` (actually voice release): `memset(slot, 0, 0x64)` = 100 bytes cleared.

Evidence: `sub_4426A0` (0x4426A0, voice channel setup): base = `ctx_bytes + 655264` = `&ctx[163816]`, voice slots at `base + 1808` = `&ctx[164268]`.

### Audio volume state (ctx[164901–164914]):

All volume fields initialized to 5000 by `VmCtx_Init` (0x44D320). Written to system.dat by `VmTask_WriteSystemDat` (0x44D070). Byte range: task_data[165107..165120].

| DWORD index | task_data idx | PalSoundSetVolume type | Purpose |
|-------------|--------------|----------------------|---------|
| ctx[164901] | 165107 | (stored, not init-set) | vol_general (5000 default) |
| ctx[164902] | 165108 | type 3 = BGM | vol_bgm (5000 default) |
| ctx[164903] | 165109 | type 1 = default/general | vol_default (5000 default) |
| ctx[164904] | 165110 | — | 0 |
| ctx[164905] | 165111 | type 5 = Voice | vol_voice (5000 default) |
| ctx[164906] | 165112 | type 4 = SE | vol_se (5000 default) |
| ctx[164907] | 165113 | type 6 = Movie | vol_movie (5000 default) |
| ctx[164908..164914] | 165114..165120 | — | flags (1 each) |

Note: volume scale 5000 = 50% of max 10000 (matching PalSoundSlot.volume default of 10000).
ctx[164909]/[164910] = BGM/voice multipliers from VmExtcall evidence (not set by VmCtx_Init — may be loaded from system.dat).

Computed (BGM): `effective_bgm_vol = 100 * arg * ctx[164909]`.

Evidence: `VmCtx_Init` (0x44D320), `VmExtcall_BgmSetVolume` (0x40C2B0), `VmExtcall_VoiceSetVolume` (0x4445F0).

---

## Region 8: Sprite Slot Table (ctx[166597–177682])

Stride: 86 DWORDs (344 bytes) per slot. 129 slots (indices 1–0x81, max). Base at `ctx[166597]` (byte offset 666388).

```
Slot i: ctx[166597 + 86*i]
```

| Slot-relative index | Purpose |
|--------------------|---------|
| slot[0] | PAL sprite handle (or 0 if unused) |
| slot[5] | surface/sub-handle flag (nonzero → call VmSpr_ReleaseSurface) |

When slot freed:
- If `slot[5]` nonzero: `VmSpr_ReleaseSurface(slot[0])` (0x449220)
- Else if `slot[0]` nonzero: `VmSpr_FreeSlot(&ctx[166597 + 86*i])` (0x4233F0)

Maximum slot index checked: `v4 + 1 > 0x81` → error. VmExtcall_SpCls (0x428E20) iterates 0..127 (128 slots). Effective max slot count = 128.

Evidence: `VmExtcall_SpSet` (0x4299A0), `VmExtcall_SpCls` (0x428E20).

### Sprite motion table (global):

`gSpriteMotionTable` (0x4C0240): one entry per sprite, 756 bytes each.
Indexed by `VmSpr_SlotToAnimIdx(script_slot)` → internal motion index.

| Byte offset within entry | Purpose |
|--------------------------|---------|
| +16 | x position (int) |
| +20 | y position (int) |
| +24 | z / depth (int) |
| +108 | dx velocity (float) |
| +112 | dy velocity (float) |
| +116 | dz velocity (float) |

Evidence: `VmExtcall_SpSetPosMove` (0x428640).

---

## Region 8b: Script Data File Buffer Table (ctx[177637–177644])

Initialized by `VmTask_InitScript` (0x445D60). Stored just before the VM stack.

| DWORD index | byte offset | Purpose |
|-------------|-------------|---------|
| ctx[177637] | 710548 | file_dat_buf: PalMemoryAlloc'd buffer for File.dat |
| ctx[177638] | 710552 | text_dat_buf: PalMemoryAlloc'd buffer for Text.dat |
| ctx[177639] | 710556 | mem_dat_buf: PalMemoryAlloc'd buffer for Mem.dat |
| ctx[177640] | 710560 | point_dat_buf: PalMemoryAlloc'd buffer for Point.dat (also = call_frame_arg_base) |
| ctx[177641] | 710564 | file_dat_size |
| ctx[177642] | 710568 | text_dat_size |
| ctx[177643] | 710572 | mem_dat_size |
| ctx[177644] | 710576 | point_dat_size (also = call_frame_arg_offset) |

Note: ctx[177640]/[177644] serve double duty as call-frame argument base/offset (confirmed by VmExtcall_CreateThread analysis, session 3).

All files decrypted if first byte == '$' (0x24): `DataDecrypt(buf+16, size-16)`.

---

## Region 9: VM Stack (ctx[177646–177709])

64-element stack of 32-bit values.

| DWORD index | Purpose |
|-------------|---------|
| ctx[177646 + i] | stack element i (i=0..63) |

Stack is accessed via ctx[184628] as top pointer (one-past-end).

---

## Region 10: VM Variables (ctx[177710–243245])

65536 VM variables, each a 32-bit signed integer.

| DWORD index | Purpose |
|-------------|---------|
| ctx[177710 + var_idx] | VM variable at index var_idx (0..65535) |

Extcall return value written to: `ctx[ctx[184629] + 177710]`.

Evidence: `VmExtcall_BgmPlay` return writeback.

---

## Region 10b: Debug Data Pointer Array (ctx[184623–184627])

Set by `VmTask_InitScript` (0x445D60) via `PalDebugSetData`. 5-element array of pointers into ctx.

| DWORD index | byte offset | Points to | PalDebugSetData id | Count |
|-------------|-------------|-----------|-------------------|-------|
| ctx[184623] | 738492 | &ctx[177965] | 11 | 1024 |
| ctx[184624] | 738496 | &ctx[178989] | 13 | 256 |
| ctx[184625] | 738500 | &ctx[183085] | 12 | 1024 |
| ctx[184626] | 738504 | &ctx[184109] | 17 | 1 |
| ctx[184627] | 738508 | &ctx[184110] | 18 | 1 |

Also: audio list pointers registered via PalDebugSetData:
- ctx[163816] = &ctx[163822] (SE slot array head, id 14, count 496)
- ctx[163817] = &ctx[164206] (BGM slot array head, id 15, count 62)

---

## Region 11: Stack Control / Extcall Args (ctx[184628–184629])

| DWORD index | byte offset | Purpose |
|-------------|-------------|---------|
| ctx[184628] | 738512 | stack_top_idx (0..64) |
| ctx[184629] | 738516 | extcall_arg: destination variable index for return value |

Written by `VmOpc_Extcall` (0x42CF50): `ctx[184629] = instr2` (second DWORD of extcall instruction).

---

## Region 12: Inline String Storage (ctx_bytes+771296–)

Used for script-defined string literals embedded in task data.

```c
char *slot = (char *)(PalTaskGetTaskData(0) + 2047 * slot_idx + 772120)
           = ctx_bytes + 2047 * slot_idx + 771296
```

Slot size: 2047 bytes. Addressed when resource ID has bit 0x10000000 set.

Evidence: `VmStr_GetFilename` (0x44B1E0).

---

## Region 13: Script Execution State (ctx[201012–201071])

| DWORD index | byte offset | Purpose |
|-------------|-------------|---------|
| ctx[201012] | 804048 | saveload_list: PalList* created by VmTask_InitScript (save/load state list) |
| ctx[201013] | 804052 | script_base: pointer to Script.src buffer (PalMemoryAlloc'd by VmTask_InitScript) |
| ctx[201014] | 804056 | script_buf_size: size of Script.src buffer |
| ctx[201015] | 804060 | pc: current program counter (relative to script_base; init from script header[8..11]) |
| ctx[201016] | 804064 | script_header_size: 12 (header bytes before script data; base offset for jumps) |
| ctx[201017] | 804068 | script_entry_count: value from script header[8..11] (also initial PC target) |
| ctx[201018] | 804072 | cur_extcall_id: packed [cat:16][idx:16] |
| ctx[201021] | 804084 | blocking_flag: 1 = this tick is blocked (wait in progress) |
| ctx[201056] | 804224 | saved_pc_trace: PC snapshot for debug tracing |
| ctx[201057] | 804228 | history_list: PalList* for return stack in history replay |
| ctx[201058] | 804232 | scheduling substate (cleared by VmState_ResetScheduling) |
| ctx[201059] | 804236 | scheduling substate (cleared) |
| ctx[201062] | 804248 | scheduling mode byte (cleared) |
| ctx[201063] | 804252 | scheduling mode dword (cleared) |
| ctx[201064] | 804256 | scheduling_mode_flag (read by VmScriptRunStep for stop condition) |
| ctx[201066] | 804264 | history_skip_active: nonzero = currently skipping for history replay |
| ctx[201067] | 804268 | history_skip_end_pc: PC where history skip ends |
| ctx[201022] | 804088 | skip_effects_flag: if nonzero, VmTask_TickEffects returns immediately |
| ctx[201061] | 804244 | key_push_state: `\|= PalInputGetKeyPush()` each tick (accumulated key events) |
| ctx[201080] | 804320 | effects_active_bitmask: outer tracking of active effects (bit 1 = timed transition) |
| ctx[201081] | 804324 | scheduler_pending_mask: bitmask of effects awaiting completion |
| ctx[201082] | 804328 | shake_type (cleared on done, OWORD-zeroed with 201083..201085) |
| ctx[201083] | 804332 | shake_start_time (PaltimeGetTime() snapshot) |
| ctx[201084] | 804336 | shake_duration (ms; 0xFFFFFFFF = infinite) |
| ctx[201085] | 804340 | shake_axis (0=both_random, 1=x_only, 2=y_only, 3=both_separate) |
| ctx[201086] | 804344 | shake_amplitude (half-range; v2 = 2×this) |
| ctx[201087] | 804348 | (cleared with 201086 via QWORD store) |
| ctx[201088] | 804352 | shake_flags (bit 0 = apply to focus sprite ctx[1067]; bit 1 = random_mode2) |
| ctx[201091] | 804364 | effect2_timer_start (timed transition start timestamp) |
| ctx[201092] | 804368 | effect2_timer_duration (timed transition duration in ms) |
| ctx[201229..201260] | 804916..805040 | point_stack[0..31]: 32-entry script call-point stack |
| ctx[201261] | 805044 | point_stack_depth: current depth; decremented on pop |

Focus sprite pointer: ctx[1067] (byte 4268) = focus_sprite_ptr for shake effect (PalSprite handle targeted by shake_flags bit 0).

### Effect type 3 slot table (ctx[201093–203132]):

Base at `ctx_bytes + 804372`. 255 sprite slots × 8 DWORDs each.

| Slot-relative index | Purpose |
|--------------------|---------|
| slot[0] | PalSprite handle |
| slot[3] | flags (bit 24 set = skip this slot) |
| slot[4..5] | initial x/y position |
| slot[6..7] | target x/y position |

| DWORD index | Purpose |
|-------------|---------|
| ctx[203133] | effect3_status (2 = active fade; cleared on done) |
| ctx[203135] | effect3_start_time |
| ctx[203136] | effect3_duration |
| ctx[203139] | effect3_sprite_count |

On completion: all 255 sprites released via PalSpriteRelease(PalRenderGetActive()), then memset block 0x2000 bytes.

### Effect type 4 slot table (ctx[203141–204537]):

Base at `ctx_bytes + 812564`. 127 sprite slots × 11 DWORDs each.

| Slot-relative index | Purpose |
|--------------------|---------|
| slot[0] | PalSprite handle |
| slot[5] | timer0: last-frame-advance time |
| slot[6] | timer1: per-step duration |
| slot[7] | step_count: increments each step |
| slot[8] | src_x / target_x |
| slot[9] | src_y / target_y |
| slot[10] | current_frame (WORD, cycles 0..7 via PalSpriteRectSetPos) |

| DWORD index | Purpose |
|-------------|---------|
| ctx[204538] | effect4_status (2 = active; cleared on done) |
| ctx[204539] | effect4_start_time |
| ctx[204540] | effect4_duration |
| ctx[204543] | effect4_sprite_count |

On completion: 127 sprites released, memset block 0x15F0 bytes.

Evidence: `VmEffect_TickShake` (0x412AC0), `VmEffect_TickSlideFade` (0x412860), `VmEffect_TickRectAnim` (0x412620).

Script.src format: `[0..3]` = magic "PSG0" (0x50534730); `[8..11]` = initial PC; `[12..]` = script data.

VmScriptRunStep stop condition: `(VmGetTaskSchedulingState() & v11) != 0 || !step_flag`:
- v11 = 16 if `ctx[201064]` nonzero else 32.

Evidence: `VmScriptRunStep` (0x44A2F0), `VmState_ResetScheduling` (0x44AC90), `VmTask_InitScript` (0x445D60), `VmTask_Tick` (0x4457F0).

---

## Region 14: Action Slot State (ctx[204567+], stride 74055 DWORDs)

16 action slots, each 296220 bytes apart in ctx.

```
Slot i base (dword*): ctx_bytes + 296220 * i
Slot i, field[204567]: ctx[204567 + 74055*i]  — active flag
Slot i, field[204572]: ctx[204572 + 74055*i]  — first arg (script ID)
Slot i, field[204573]: ctx[204573 + 74055*i]  — run flag (→1 on start)
```

Default active slot index at `ctx[204550]` (byte 818200). When script passes -1 as slot index, uses ctx[204550].

Helpers called on start:
- `VmAction_Init(&slot[204567])` (0x408C40)
- `VmAction_Apply(&slot[204567])` (0x408B00) — called unless skip/auto mode

Evidence: `sub_40B6E0` / VmExtcall_ActionRun (cat17[0]).

---

## Region 15: Thread Subsystem State (ctx[1,389,451+])

Thread state immediately follows action slots (action slot 15 ends at ctx[1,389,446]; 4-DWORD gap).

All fields accessed via `[esi + 0x54CEXX]` displacement pattern where esi=ctx_bytes.

```
ctx[1,389,451]  (byte 0x54CE2C)  ctx_off_thread_active   — set 1 by CreateThread, cleared by ExitThread
ctx[1,389,452]  (byte 0x54CE30)  ctx_off_thread_running  — SuspendThread reads (returns old) and writes new value
ctx[1,389,453]  (byte 0x54CE34)  (gap / unknown field)  — cleared by ExitThread OWORD write
ctx[1,389,454]  (byte 0x54CE38)  ctx_off_thread_target_pc — script label/PC from call-frame arg lookup
ctx[1,389,455]  (byte 0x54CE3C)  ctx_off_thread_id       — thread ID (returned by GetThread)
ctx[1,389,472]  (byte 0x54CE80)  ctx_off_adv_msg_active  — if nonzero with adv_msg_pc: trigger AdvMessageProc
ctx[1,389,473]  (byte 0x54CE84)  ctx_off_adv_msg_pc      — target PC for auto-advance message jump
```

Handler evidence:
- CreateThread (0x42E900): sets _active=1, _running=1, _target_pc=arg_lookup, _id=stack_arg
- ExitThread   (0x42E8C0): OWORD-clears [1389451..1389454], then clears [1389455]
- SuspendThread(0x42E860): reads running→return var, writes stack_arg→running
- GetThread    (0x42E9A0): returns _id field

Call-frame argument lookup pattern (ctx[177640], ctx[177644]):
- ctx[177640]: call_frame_arg_base (pointer)
- ctx[177644]: call_frame_arg_offset
- arg[v2] = ctx[177640] + ctx[177644] - 4*v2  (backward-indexed from end)

---

## Region 16: Script Data Pointers (ctx[1,439,026+])

Accessed via `(char *)&unk_57D8XX + task_data_ptr` pattern in `VmTask_Main` (0x4050A0).
These are task_data-relative, but equivalent to ctx at `task_data_byte - 824`:

```
task_data[0x57D800]  ctx_dword[1,439,026]  ctx_off_script_listhead  → PalListCreate() result
task_data[0x57D804]  ctx_dword[1,439,027]  ctx_off_script_buf2      → 2nd script heap ptr
task_data[0x57D808]  ctx_dword[1,439,028]  ctx_off_script_buf1      → 1st script heap ptr (from system.dat)
task_data[0x57D80C]  ctx_dword[1,439,029]  ctx_off_script_buf2_size → size of buf2 (read from system.dat)
task_data[0x57D810]  ctx_dword[1,439,030]  ctx_off_script_buf1_size → size of buf1 (read from system.dat)
```

VmTask_Main load sequence from system.dat:
1. `ReadFile(fd, task_data, 0x330)` → header (effects mode @[4], window mode @[2], cursor hide @[9])
2. `ReadFile(fd, task_data+733164, 0x1000)` → script image (at ctx_byte[732340])
3. Read `buf1_size` from file → read buf1 bytes → `ctx[1,439,028]`
4. Read `buf2_size` from file → read buf2 bytes → `ctx[1,439,027]`

Evidence: VmTask_Main (0x4050A0) xref to gGameMetadata+1040 (save path).

---

## ctx Total Size

ctx extends to at least byte offset **5,756,124** bytes (from ctx_dword[1,439,030]).
Task data buffer = 824 + 5,756,124 = **~5,756,948 bytes** (~5.5 MB total).

---

## Task Data Layout (from task_data base, not ctx)

```
[0..823]     : PAL task header / scheduling state
[824..]      : ctx begins here
[772120..]   : inline string area (from PalGetTaskData(0)+772120)
```

Note: `PalTaskGetTaskData(0)[4]` and `[9592/4]` are checked for thumbnail flags (task header fields, before ctx starts).

---

## Globals Referenced by Game.exe

| Address | Name | Purpose |
|---------|------|---------|
| 0x4BD708 | unk_4BD708 | linked list head for thread/node chain |
| 0x4BD6F0 | unk_4BD6F0 | buffer (0xE0=224 bytes) — cleared by VmState_ResetScheduling |
| 0x49C3E0 | dword_49C3E0 | icon slot → sprite layer mapping table |
| 0x49C400 | gSineTable | 512-entry DWORD sine/cosine lookup table; used by VmEffect_TickSlideFade/TickRectAnim for X oscillation: `delta_x - gSineTable[frame & 0x1FF] * -50.0f` |
| 0x49BE40 | off_49BE40 | array of audio extension strings (.ogg variants) |
| 0x4BB1D0 | gOpcodeDispatchTable | opcode handler table (132 × VmExtcallHandler) |
| 0x49C360 | gExtcallCategoryTable | extcall category table (24 × VmExtcallHandler*) |
