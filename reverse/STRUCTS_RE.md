# Recovered Struct Definitions

## Evidence Key

| Tag | Source |
|-----|--------|
| `decompile` | IDA decompiler pseudocode with address |
| `writeup` | `docs/writeup.md` prior analysis |
| `runtime` | Rust re-implementation (`crates/pal-vm/src/runtime.rs`) |
| `changelog` | IDB_CHANGELOG.md or session notes |

See `ENGINE_CONTEXT_RE.md` for the full ScriptContext field map, and `PARAMETER_PASSING_RE.md` for calling conventions.

---

## ScriptExecFrame (Game.exe — `VmScriptRunStep` this-pointer)

`VmScriptRunStep` (0x44A2F0) is `__thiscall` on `ScriptExecFrame *`.

Evidence: decompile of `VmScriptRunStep`.

```c
struct ScriptExecFrame {
    /*[0x00]*/ DWORD script_base;     // base pointer of loaded script buffer
    /*[0x04]*/ DWORD exec_limit;      // execution end addr / tick limit (this[1])
    /*[0x08]*/ DWORD pc;              // program counter (this[2])
    /*[0x0C]*/ DWORD field_0C;        // unknown (this[3])
    /*[0x10]*/ DWORD breakpoint_count;// this[4].pc used as breakpoint table length
    /*[0x14]*/ DWORD cur_instr;       // current fetched instruction word (this[5])
    /*[0x18]*/ DWORD field_18;        // unknown (this[6])
    /*[0x1C]*/ DWORD running_flag;    // 1 while executing (this[7])
    /*[0x20]*/ DWORD step_flag;       // 0=continue, 1=stop after this instr (this[8])
    /*[0x24]*/ DWORD blocked_flag;    // nonzero = suspended, skip execution (this[9])
    /*[0x28]*/ DWORD field_28;        // unknown (this[10])
    /*[0x2C]*/ DWORD field_2C;        // unknown (this[11])
    // gap
    /*[0xA8]*/ DWORD sub_frame_ptr;   // pointer to sub-frame or parent context (this[42])
};
```

Field-level evidence:

| Field | Offset | Access | Evidence address |
|-------|--------|--------|-----------------|
| `script_base` | 0x00 | read | VmScriptRunStep 0x44a370 `v7 = this->pc; script_base = this->script_base` |
| `exec_limit` | 0x04 | read | 0x44a323 `if (pc == this->exec_limit) break` |
| `pc` | 0x08 | rw | 0x44a31d `pc = this->pc`, 0x44a384 `this->pc = v10` |
| `cur_instr` | 0x14 | write | 0x44a387 `this->cur_instr = v9` |
| `running_flag` | 0x1C | write | 0x44a316 `this->running_flag = 1` |
| `step_flag` | 0x20 | rw | 0x44a372 `this->step_flag = 0`, loop condition |
| `blocked_flag` | 0x24 | read | 0x44a306 `if (this->blocked_flag) return 1` |

---

## ScriptContext (Game.exe — extcall handler argument)

`ctx = (unsigned int *)(PalTaskGetTaskData(0) + 824)`

All offsets are DWORD indices from ctx (multiply by 4 for byte offset).

Full layout documented in `ENGINE_CONTEXT_RE.md`. Key fields with evidence:

| DWORD index | Byte offset | Field name | Evidence | Source address |
|-------------|-------------|------------|---------|---------------|
| ctx[1] | 4 | text_active | `if (ctx[1])` | VmExtcall_TextSetIcon 0x440c3e |
| ctx[2] | 8 | cleared on reset | `this+8 = 0` | VmState_ResetScheduling 0x44ad3c |
| ctx[4] | 16 | text_init_param0 | pop from stack | VmExtcall_TextInit 0x44143f |
| ctx[5] | 20 | text_init_param7 | pop from stack | VmExtcall_TextInit 0x4415a5 |
| ctx[6] | 24 | text_pos_x | pop from stack | VmExtcall_TextInit 0x44146d |
| ctx[7] | 28 | text_pos_y | pop from stack | VmExtcall_TextInit |
| ctx[8] | 32 | text_flags | pop, alignment bits 0x80000003 | VmExtcall_TextInit 0x4415b6 |
| ctx[9-11] | 36-44 | text_params | pops from stack | VmExtcall_TextInit |
| LOWORD(ctx[12]) | 48 | init_flag = -1 | `*(WORD*)ctx+24 = -1` | VmExtcall_TextInit 0x4415ad |
| ctx[1061] | 4244 | text_sub_param0 | `ctx[1061] = ctx[6]` | VmExtcall_TextInit 0x4415d2 |
| ctx[1062] | 4248 | text_sub_param1 | `ctx[1062] = ctx[7]` | VmExtcall_TextInit 0x4415db |
| ctx[1063] | 4252 | text_task_handle | freed on reset | VmState_ResetScheduling 0x44ad26 |
| ctx[1068+2i] | 4272+8i | icon_sprite[i] | `ctx[2*v13+1068]` | VmExtcall_TextSetIcon 0x440c42 |
| ctx[1069+2i] | 4276+8i | icon_anim[i] | PalAnimationRelease | VmExtcall_TextSetIcon 0x440c5c |
| ctx[2192] | 8768 | save_base | accessed by VmSaveState_AdvStack | 0x42f868 |
| ctx[2193] | 8772 | save_frame_w | PalThumbnailCreate arg | 0x42f881 |
| ctx[2194] | 8776 | save_frame_h | PalThumbnailCreate arg | 0x42f88d |
| ctx[2195] | 8780 | save_active | if-check for thumbnail | 0x42f861 |
| ctx[2196] | 8784 | thumb_width | PalThumbnailCreate arg | 0x42f8b9 |
| ctx[2197] | 8788 | thumb_height | PalThumbnailCreate arg | 0x42f8b9 |
| ctx[2200] | 8800 | saved_pc | for history replay | VmScriptRunStep 0x44a46b |
| ctx[2207] | 8828 | effect_active | set to 1 | VmScriptRunStep 0x44a4d5 |
| ctx[2208] | 8832 | skip_thumbnail | mode check | VmScriptRunStep 0x44a44c |
| ctx[2210] | 8840 | history_counter | zeroed on replay | VmScriptRunStep 0x44a471 |
| ctx[2211] | 8844 | history_counter_saved | `ctx[2211]=ctx[2210]` | VmScriptRunStep 0x44a4be |
| ctx[3240] | 12960 | effect_start_ms | PaltimeGetTime() | VmScriptRunStep 0x44a4e5 |
| ctx[3241] | 12964 | effect_duration_ms | = 100 | VmScriptRunStep 0x44a4eb |
| ctx[22083] | 88332 | history_gosub_target | gosub point ID | VmScriptRunStep 0x44a499 |
| ctx[163809] | 655236 | wait_start_time | PaltimeGetTime() | VmExtcall_Wait 0x444f63 |
| ctx[163810] | 655240 | wait_duration_ms | set on first call | VmExtcall_Wait 0x44500b |
| ctx[163814] | 655256 | wait_skip_flag | enables auto-skip | VmExtcall_Wait 0x44503f |
| ctx[163815] | 655260 | wait_once_flag | 0=first, nonzero=continuing | VmExtcall_Wait 0x444fbb |
| ctx[164206] | 656824 | bgm_slot0_base | `ctx[164206..164236]` | VmExtcall_BgmPlay 0x40c660 |
| ctx[164237] | 656948 | bgm_slot1_base | `ctx[164237..164267]` | VmExtcall_BgmPlay 0x40c64b |
| ctx[166597+86i] | 666388+344i | sprite_slot_base[i] | `ctx[86*v4+166597]` | VmExtcall_SpSet 0x429a96 |
| ctx[177646+i] | 710584+4i | stack_value[i] | pop pattern | all handlers |
| ctx[184628] | 738512 | stack_top_idx | read/written by all | all handlers |
| ctx[184629] | 738516 | extcall_arg / return_dst | set by VmOpc_Extcall | 0x42cf79 |
| ctx[201013] | 804052 | script_base_ptr | `v2 = ctx[201013]` | VmOpc_Extcall 0x42cf5d |
| ctx[201015] | 804060 | pc | rw, also rewound by -12 | all handlers |
| ctx[201016] | 804064 | script_base_offset | `new_pc = target + ctx[201016]` | VmScriptRunStep 0x44a54c |
| ctx[201018] | 804072 | cur_extcall_id | [cat:16][idx:16] packed | VmOpc_Extcall 0x42cf84 |
| ctx[201021] | 804084 | blocking_flag | set 1 by wait handlers | VmExtcall_Wait 0x444f4f |
| ctx[201056] | 804224 | saved_pc_trace | debug snapshot | VmScriptRunStep 0x44a349 |
| ctx[201057] | 804228 | history_list_ptr | PalList* | VmScriptRunStep 0x44a40e |
| ctx[201058] | 804232 | sched_substate0 | cleared on reset | 0x44acc5 |
| ctx[201059] | 804236 | sched_substate1 | cleared on reset | 0x44accf |
| ctx[201062] | 804248 | sched_mode_byte | cleared (byte) | 0x44acbe |
| ctx[201063] | 804252 | sched_mode_dword | cleared | 0x44acb4 |
| ctx[201064] | 804256 | sched_mode_flag | controls step vs. cont. | VmScriptRunStep 0x44a3d6 |
| ctx[201066] | 804264 | history_skip_active | nonzero while skipping | VmScriptRunStep 0x44a34f |
| ctx[201067] | 804268 | history_skip_end_pc | PC where skip ends | VmScriptRunStep 0x44a358 |
| ctx[204550] | 818200 | action_default_idx | default slot (-1→this) | VmExtcall_ActionRun 0x40b751 |
| ctx[204567+74055i] | 818268+296220i | action_slot_active[i] | per-slot active flag | VmExtcall_ActionRun 0x40b75f |
| ctx[204572+74055i] | 818288+296220i | action_slot_arg0[i] | first script arg | VmExtcall_ActionRun 0x40b768 |
| ctx[204573+74055i] | 818292+296220i | action_slot_run_flag[i] | set to 1 on start | VmExtcall_ActionRun 0x40b76e |

---

## BgmSlot (Game.exe, embedded in ScriptContext)

31-DWORD slot, 2 instances. Base at `ctx[164206]` (byte 656824). Stride 31 DWORDs.

```c
struct BgmSlot {                // 124 bytes = 31 DWORDs
    /*[0x00]*/ int handle;      // PAL sound handle (from VmSnd_LoadOgg/PalSoundLoad)
    /*[0x04]*/ int res_id;      // resource ID / name index
    /*[0x08]*/ int flags;       // bit0=?, bit4=loop, bit28=inline string ID
    /*[0x0C]*/ int volume;      // volume percent (100 = full)
    /*[0x10..0x1C]*/ DWORD field_10[3]; // unknown
    /*[0x20]*/ DWORD field_20[4]; // (8..15) filename string copy (16 bytes)
    /*[0x40]*/ DWORD field_40[8]; // (16..23) filename cont. (32 bytes)
    /*[0x60]*/ int loop_flag;   // flags & 0x10
    /*[0x64]*/ int fade_in_ms;
    /*[0x68]*/ int fade_out_ms;
    /*[0x6C]*/ int param3;
    /*[0x70..0x7C]*/ DWORD reserved[3];
};
```

Fields (8..23) form the 64-byte filename buffer populated by `VmStr_GetFilename`.

Evidence: `VmExtcall_BgmPlay` (0x40C4B0), slots accessed at 0x40c660.

---

## PalSpriteSlot (Game.exe, embedded in ScriptContext)

86-DWORD slot (344 bytes), 129 instances (indices 0–128). Base at `ctx[166597]` (byte 666388). Stride 86 DWORDs. Total size: `memset(slot, 0, 0x158)` = 344 bytes confirmed.

Access via `&ctx[86 * slot_idx + 166597]` (slot_ptr = byte pointer to slot start).

```c
struct PalSpriteSlot {          // 0x158 = 344 bytes
    /*[0x00]*/ void *surface;   // PAL sprite surface/handle; released via VmSpr_ReleaseHandle
    /*[0x04]*/ void *sub_obj;   // sub-object ptr (linked list node: [3]=data, [4]=next, [5]=prev)
    /*[0x08]*/ DWORD field_08;  // unknown
    /*[0x0C]*/ DWORD flags;     // bit 0x800000 = async load in progress
    /*[0x10]*/ DWORD mode;      // if 1: use alt_handle instead of surface on free
    /*[0x14]*/ int surface_flag;// (from ctx[5*slot+166597+5]) nonzero → surface path
    /*[0x18]*/ DWORD transition_handle; // PalSpriteReleaseTransitionHandle arg
    /*[0x1C]*/ DWORD field_1C;  // unknown
    /*[0x20]*/ DWORD alt_handle;// if mode==1: written to *surface on free
    // ... more fields up to 0x158
};
```

Fields from VmSpr_FreeSlot (0x4233F0):
- `slot+0x00` (`*slot` as ptr): surface — released via `VmSpr_ReleaseHandle` then `PalSpriteRelease`
- `slot+0x04` (`slot[1]` as ptr): sub_obj — linked list node; fields [3]=heap-data, [4]=next, [5]=prev-related
- `slot+0x0C` (`slot[3]`): flags — bit 0x800000 = loading in progress (spins waiting)
- `slot+0x10` (`slot[4]`): mode — if == 1, deref surface→write alt_handle before release
- `slot+0x18` (`slot[6]`): transition handle — `PalSpriteReleaseTransitionHandle(slot[6])`
- `slot+0x20` (`slot[8]`): alt_handle — used when mode==1

Note: VmExtcall_SpSet checks `slot[5]` (not `slot+0x14`/field_14) for surface_flag. `slot[5]` = byte offset 0x14 = 20. So surface_flag is at offset 20.

From VmExtcall_SpCls (0x428E20): clears slots 0..127 (128 slots active, 0-based).

When freeing (from VmExtcall_SpSet 0x4299A0):
- If `ctx[166597 + 86*slot + 5]` nonzero: `VmSpr_ReleaseSurface(ctx[166597 + 86*slot])`
- Else if `ctx[166597 + 86*slot]` nonzero: `VmSpr_FreeSlot(&ctx[166597 + 86*slot])`

Evidence: `VmExtcall_SpSet` (0x4299A0), `VmSpr_FreeSlot` (0x4233F0), `VmExtcall_SpCls` (0x428E20).

## SpriteMotionEntry (Game.exe — `gSpriteMotionTable`)

Global table at `gSpriteMotionTable` (0x4C0240). 756-byte entries per sprite motion slot.

| Byte offset | Field |
|-------------|-------|
| +16 | x position (int) |
| +20 | y position (int) |
| +24 | z/depth (int) |
| +108 | dx velocity (float) |
| +112 | dy velocity (float) |
| +116 | dz velocity (float) |

Indexed by `VmSpr_SlotToAnimIdx(script_slot_idx)` → internal motion slot index.
`SpSetPosMove` adds delta (arg * 1.5) to position and velocity fields simultaneously.

Evidence: `VmExtcall_SpSetPosMove` (0x428640) at 0x428750–0x42878f.

---

## PalTask (PAL_d.dll — task node)

Evidence: `writeup.md`, PAL_d.dll debug symbols.

```c
struct PalTask {               // 0x2C = 44 bytes
    /*[0x00]*/ PalTask *next;      // next sibling
    /*[0x04]*/ PalTask *prev;      // previous sibling
    /*[0x08]*/ PalTask *child;     // first child
    /*[0x0C]*/ DWORD state;        // task state flags
    /*[0x10]*/ DWORD flags;        // bit 0x10000 = blocking flag
    /*[0x14]*/ DWORD task_data;    // pointer to game-allocated data
    /*[0x18]*/ DWORD proc;         // main process fn ptr
    /*[0x1C]*/ DWORD proc_next;    // next-frame process fn ptr
    /*[0x20]*/ DWORD proc_end;     // cleanup fn ptr
    /*[0x24]*/ DWORD user_data;    // user word
    /*[0x28]*/ DWORD msg;          // message
};
```

512-slot task pool. ScriptContext base = `task_data + 824`.

---

## PalSprite (PAL_d.dll)

Evidence: `PalSpriteCreate_0` (0x1025C190), PAL_d.dll debug symbols.

```c
struct PalSprite {             // 0x190 = 400 bytes
    /*[0x00]*/ DWORD surface_handle;  // render surface (from sub_1011D27B or sub_10118348)
    /*[0x04]*/ DWORD field04;
    /*[0x08]*/ DWORD field08;
    /*[0x0C]*/ DWORD field0C;
    /*[0x10]*/ DWORD field10;
    /*[0x14]*/ DWORD field14;
    /*[0x18]*/ DWORD field18;
    /*[0x1C]*/ DWORD anim_handle;      // released by sub_1011CEED in PalSpriteRelease_0
    /*[0x20]*/ DWORD render_handle;   // [+8] PalRenderInsert result
    /*[0x24]*/ DWORD field24;
    /*[0x28]*/ DWORD state;           // [+10] = 0 on create
    /*[0x2C]*/ RECT  src_rect;        // [+11..14] source rect: {0,0,width,height} (16 bytes)
    /*[0x3C]*/ DWORD color;            // [+15] set to color arg by PalSpriteSetColor_0; PalSpriteCreate_0 writes -1 as sentinel
    /*[0x40]*/ DWORD x;               // [+16] set by PalSpriteSetPos arg a2
    /*[0x44]*/ DWORD y;               // [+17] set by PalSpriteSetPos arg a3
    /*[0x48]*/ DWORD z;               // [+18] depth/z-order (PalSpriteSetPos arg a4); used as layer+z offset
    /*[0x4C]*/ DWORD field4C[2];      // [+19..20] unknown
    /*[0x54]*/ DWORD width;           // [+21] = xRight param
    /*[0x58]*/ DWORD height;          // [+22] = yBottom param
    /*[0x5C]*/ DWORD width_copy;      // [+23] = width
    /*[0x60]*/ DWORD height_copy;     // [+24] = height
    /*[0x64]*/ DWORD field64[2];      // [+25..26] unknown
    /*[0x6C]*/ DWORD layer;           // [+27] = a4 (layer/priority param)
    /*[0x70]*/ float scale;           // [+28] = 1.0f (0x3F800000) on create; set by PalSpriteSetScale_0
    /*[0x74]*/ DWORD field74;         // [+29] = 0
    /*[0x78]*/ DWORD field78;         // [+30] = 0
    /*[0x7C]*/ DWORD field7C;         // [+31] = 0
    /*[0x80]*/ DWORD field80;         // [+32] = 1
    // ... remaining 0x110 bytes: x/y pos, scale, rotation, etc.
};
```

Note: offset [+32] to [+96] contain transform fields; total struct 0x190 = 400 bytes.

---

## PalSoundSlot (PAL_d.dll — embedded in Block global, all types)

Evidence: `PalSoundLoad_0` (0x10255370). Each slot is 88 bytes = 22 DWORDs. All sound state lives in the Block global (0x103E1A30), NOT in Game.exe ctx.

| Sound type a2 | Block offset | Count | Handle high bits | Likely purpose |
|---------------|--------------|-------|-----------------|----------------|
| default/1 | +1496 | 16 | 0x30000000 | General |
| 3 | +2904 | 2 | 0x20000000 | BGM (2 slots matches ctx BGM slots) |
| 2 | +3080 | 8 | 0x70000000 | unknown |
| 4 | +3784 | 16 | 0x40000000 | SE (16 slots matches ctx SE slots) |
| 5 | +5192 | 64 | 0x50000000 | Voice (64 PAL slots for 8 game slots) |
| 6 | +10824 | 16 | 0x60000000 | Movie audio |

Sound handle encoding: `handle = (type_high_bits) | slot_index`.

```c
struct PalSoundSlot {          // 88 bytes = 22 DWORDs
    /*[0x00]*/ void  *dsound_buf;   // IDirectSoundBuffer COM vtable pointer (confirmed PalSoundPlay_0 vtbl dispatch)
    /*[0x04]*/ DWORD  field_04;
    /*[0x08]*/ DWORD  play_state;   // playback state flags (from PalSoundPlay_0)
    /*[0x0C]*/ DWORD  loop_flag;    // 0=one-shot, 1=loop (from PalSoundPlay_0)
    /*[0x10]*/ DWORD  active;       // nonzero → PalSoundRelease slot before reload
    /*[0x14]*/ DWORD  field_14;
    /*[0x18]*/ DWORD  field_18;
    /*[0x1C]*/ DWORD  field_1C;
    /*[0x20]*/ DWORD  field_20;
    /*[0x24]*/ DWORD  field_24;
    /*[0x28]*/ DWORD  field_28;
    /*[0x2C]*/ DWORD  field_2C;
    /*[0x30]*/ DWORD  field_30;
    /*[0x34]*/ DWORD  status_code;  // 6 = loading, 4 = releasing (observed PalSoundLoad_0 / PalSoundRelease_0)
    /*[0x38]*/ DWORD  sound_type;   // a2 from PalSoundLoad: 1=default, 3=BGM, 4=SE, 5=Voice, 6=Movie
    /*[0x3C]*/ DWORD  field_3C;
    /*[0x40]*/ DWORD  field_40;
    /*[0x44]*/ DWORD  field_44;
    /*[0x48]*/ DWORD  volume;       // default 10000
    /*[0x4C]*/ DWORD  field_4C;
    /*[0x50]*/ DWORD  field_50;
    /*[0x54]*/ DWORD  play_time;    // last field (slot ends at 0x58 = 88 bytes)
};
```

Note: SeSlot/BgmSlot/VoiceSlot in ctx (see above) hold game-side script state (resource IDs, wait flags); the PAL-side audio data lives here in Block.

---

## SeSlot (Game.exe, embedded in ScriptContext)

SE (sound effect) slot. 24-DWORD (96-byte) slot. Up to 16 slots. Base at `ctx[163822]`. Stride 24 DWORDs.

```c
struct SeSlot {                // 96 bytes = 24 DWORDs
    /*[0x00]*/ int handle;     // PAL sound handle; 0 = unused
    /*[0x04]*/ int res_id;     // resource ID
    /*[0x08]*/ int flags;      // bit5=se_wait, bit4=loop; 0x20000000 default
    /*[0x0C]*/ int param;      // loop category or playback param
    /*[0x10]*/ DWORD field_10[3]; // unknown
    /*[0x1C]*/ int cleared;    // slot[7]: set to 0 on load
    /*[0x20]*/ DWORD filename[16]; // 64 bytes: filename string if bit 0x10000000 set in res_id
    /*[0x60]*/ DWORD reserved[4]; // pad to 96 bytes
};
```

SE slot (slot_idx, 0-based): `ctx[163822 + 24 * slot_idx]`.
In SE play: slot accessed as `a1[24 * (se_wait_slot_num) + 163822]` where `se_wait_slot_num` is 1-based.

Evidence: `VmOpc_SeLoad` (0x4353C0), `VmOpc_SePlay` (0x435200), `VmSE_CheckWait` (0x434140).

## VoiceSlot (Game.exe, embedded in ScriptContext)

Voice slot. 25-DWORD (100-byte) slot. 8 slots. Base at `ctx[164268]`. Stride 25 DWORDs.

```c
struct VoiceSlot {             // 100 bytes = 25 DWORDs
    /*[0x00]*/ int handle;     // PAL sound handle; nonzero = in use
    /*[0x04..0x63]*/ DWORD data[24]; // voice slot data (filename, flags, state)
};
```

Voice slot i: `ctx[164268 + 25*i]` for i=0..7.

Context: `ctx[163819]` = bitmask, bit i set → slot i is waiting.
Release: `memset(slot, 0, 0x64)` = 100 bytes.

Evidence: `VmExtcall_VoicePlay` (0x4447B0), `VmVoice_ReleaseChannel` (0x4426A0), `VmVoice_CheckWait` (0x442280).

---

## ActionSubsystemState (Game.exe — per-slot, embedded in ScriptContext)

16 slots at stride 296220 bytes in ctx. Slot i active fields start at ctx_dword[204567 + 74055*i].

```c
struct ActionSlot {
    // at ctx_dword[204567 + 74055*i]:
    /*[0]*/ DWORD active_flag;    // nonzero = slot running
    /*[1..4]*/ DWORD reserved[4];
    /*[5]*/ DWORD arg0;           // script arg 0 (v5, ID)
    /*[6]*/ DWORD run_flag;       // 1 = started this tick
    // ... more fields (total slot size unknown, stride = 74055 DWORDs)
};
```

Default slot index: `ctx[204550]`.

Evidence: `sub_40B6E0` / VmExtcall_ActionRun (0x40B6E0).

---

## PalInitParams (PAL_d.dll / Game.exe — PalInit argument)

Recovered from `PalDefaultCreate_0` (0x102380A0) and `PalInit_0` (0x102385E0).
Struct is 0x6C = 108 bytes = 27 DWORDs.

```c
struct PalInitParams {         // 0x6C = 108 bytes
    /*[0x00]*/ HINSTANCE hInstance;       // [0]  set by WinMain: GetModuleHandle
    /*[0x04]*/ HWND      hSplashWnd;      // [1]  splash/setup dialog HWND (or 0)
    /*[0x08]*/ DWORD     flags2;          // [2]  misc flags (default 0)
    /*[0x0C]*/ LPCSTR    lpWindowTitle;   // [3]  default "WindowTitle"
    /*[0x10]*/ LPCSTR    lpClassName;     // [4]  default "CLASS"
    /*[0x14]*/ LPCSTR    lpIconId;        // [5]  icon resource string/ID (default 0, WinMain: 101)
    /*[0x18]*/ LPCSTR    lpMenuName;      // [6]  menu name (default NULL)
    /*[0x1C]*/ DWORD     pad7;            // [7]  (not explicitly set)
    /*[0x20]*/ DWORD     dwStyle;         // [8]  window style (default 0x2CF0000); used in CreateWindowEx
    /*[0x24]*/ DWORD     targetFPS;       // [9]  target frame rate (default 60)
    /*[0x28]*/ INT       x;              // [10]  window X pos (centered by default)
    /*[0x2C]*/ INT       y;              // [11]  window Y pos (centered by default)
    /*[0x30]*/ INT       width;          // [12]  window/render width (default 1920)
    /*[0x34]*/ INT       height;         // [13]  window/render height (default 1080)
    /*[0x38]*/ INT       renderWidth;    // [14]  = width copy
    /*[0x3C]*/ INT       renderHeight;   // [15]  = height copy
    /*[0x40]*/ DWORD     limit_a;        // [16]  = 256
    /*[0x44]*/ DWORD     limit_b;        // [17]  = 512
    /*[0x48]*/ DWORD     limit_c;        // [18]  = 2048
    /*[0x4C]*/ FARPROC   callback;       // [19]  WinMain: sub_405380; default 0
    /*[0x50]*/ DWORD     callbackArg;    // [20]  callback arg (default 0)
    /*[0x54]*/ DWORD     field21;        // [21]  from config (default 0)
    /*[0x58]*/ DWORD     visFlags;       // [22]  visibility: bit 0=hidden (no ShowWindow); default 0
    /*[0x5C]*/ DWORD     sysFlags;       // [23]  default 0x441B (17435)
    /*[0x60]*/ DWORD     field24;        // [24]  default 2
    /*[0x64]*/ DWORD     field25;        // [25]  default 1
    /*[0x68]*/ DWORD     field26;        // [26]  default 1
};
```

Evidence: `PalDefaultCreate_0` (0x102380A0), `PalInit_0` (0x102385E0), WinMain (0x405C30).

WinMain overrides: [0]=hInstance, [1]=hSplashWnd, [3][4]=MEMORY[0x4BD2D0], [5]=101, [9]=60,
                   [10-11]=XY from config.dat, [12-13]=resolution, [19]=sub_405380.

---

## PalTaskNode (PAL_d.dll — internal task node, confirmed via PalTaskCreate_0)

Confirmed 44 bytes (stride in pool). Pool base at `dword_103E1C8C`, max count at `Block[273]`.

```c
struct PalTaskNode {           // 44 bytes = 0x2C
    /*[0x00]*/ void   *data_ptr;      // heap buffer: PalMemoryAlloc(data_size, 1)
    /*[0x04]*/ DWORD   data_size;     // size passed to PalTaskCreate (=5,558,648 for main script task)
    /*[0x08]*/ FARPROC callback;      // task tick function (a1 param)
    /*[0x0C]*/ DWORD   callback_arg;  // a2 param
    /*[0x10]*/ DWORD   field10;       // (unused in create path)
    /*[0x14]*/ DWORD   param_a7;      // a7 from PalTaskCreate
    /*[0x18]*/ DWORD   type;          // a5: task type/priority (1=child, 2=sibling)
    /*[0x1C]*/ BYTE    active;        // 1=alive, 0=free (allocator checks this)
    /*[0x20]*/ DWORD   field20;       // padding / other
    /*[0x24]*/ DWORD   field24;       // padding / other
    /*[0x28]*/ PalTaskNode *sub_node; // [+40] nested task node
};
```

`PalTaskGetTaskData(taskptr)` returns `*(DWORD*)taskptr` = `data_ptr`.
`PalTaskGetTaskData(0)` → `Block[271]` → main script task node → its `data_ptr`.
ScriptContext = `data_ptr + 824`.

Block global at 0x103E1A30 (PAL_d.dll BSS), key fields:
- [155]=User32Handle, [243]=touchInitFlag, [270]=defaultParentTask,
- [271]=mainTaskNode, [273]=taskPoolMax, [274]=taskPoolCursor

---

## VmExtcallHandler (Game.exe — typedef)

```c
typedef int (__cdecl *VmExtcallHandler)(unsigned int *ctx);
```

Applied to all extcall handler table entries. Declared in IDA type library.

---

## ScriptExecFrame (IDA declared)

Struct declared in IDA. See first section above for fields.
Applied as `__thiscall` on VmScriptRunStep parameter.
