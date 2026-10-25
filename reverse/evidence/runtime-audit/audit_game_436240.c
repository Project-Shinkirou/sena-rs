// IDA evidence from the local Game.exe at 0x436240.
int __cdecl VmOpc_SelectWait(unsigned int *a1)
{
  void (*v1)(_DWORD, _DWORD, const char *, ...); // esi
  unsigned int v2; // edi
  unsigned int v3; // edi
  unsigned int v4; // edi
  int TaskData; // eax
  unsigned int v6; // edi
  int v7; // eax
  unsigned int v8; // edx
  unsigned int v9; // edx
  unsigned int *v10; // edi
  unsigned int v11; // ecx
  int v12; // eax
  unsigned int v13; // ecx
  unsigned int v14; // ecx
  int v15; // edx
  unsigned int v16; // ecx
  unsigned int v17; // ecx
  unsigned int v18; // ecx
  unsigned int v19; // ecx
  unsigned int v20; // ecx
  unsigned int v21; // ecx
  unsigned int v22; // eax
  int Active; // eax
  int Text; // eax
  unsigned int v25; // ecx
  int v26; // edi
  int v27; // esi
  int v28; // ecx
  int v29; // edx
  int v30; // esi
  char v31; // al
  char v32; // al
  int v34; // eax
  unsigned int v35; // [esp-Ch] [ebp-2ECh]
  unsigned int v36; // [esp+18h] [ebp-2C8h]
  unsigned int v37; // [esp+1Ch] [ebp-2C4h]
  unsigned int v38; // [esp+20h] [ebp-2C0h]
  int v39; // [esp+24h] [ebp-2BCh]
  unsigned int *v40; // [esp+28h] [ebp-2B8h]
  unsigned int v41; // [esp+2Ch] [ebp-2B4h]
  int v42; // [esp+30h] [ebp-2B0h]
  unsigned int v43; // [esp+34h] [ebp-2ACh]
  int v44; // [esp+38h] [ebp-2A8h]
  unsigned int *v45; // [esp+3Ch] [ebp-2A4h]
  int v46; // [esp+40h] [ebp-2A0h]
  int v47; // [esp+40h] [ebp-2A0h]
  _BYTE v48[36]; // [esp+44h] [ebp-29Ch] BYREF
  int v49; // [esp+68h] [ebp-278h]
  int v50; // [esp+6Ch] [ebp-274h]
  char v51[260]; // [esp+198h] [ebp-148h] BYREF
  char Str1[64]; // [esp+29Ch] [ebp-44h] BYREF

  v1 = (void (*)(_DWORD, _DWORD, const char *, ...))PalDebugPrintf;
  PalDebugPrintf(0, 0, "AdvCommandSelect");
  memset(v51, 0, sizeof(v51));
  if ( a1[201066] )
  {
    PalDebugPrintf(0, 0, "cancel history skip select\n pont=%x save=%x", a1[201067], a1[201069]);
    v2 = a1[201015];
    a1[201066] = 0;
    v3 = v2 - 12;
    PalDebugPrintf(0, 0, "AdvSetSavePoint");
LABEL_5:
    if ( !a1[2208] && (~(unsigned __int8)VmGetTaskSchedulingState() & 0x10) != 0 )
    {
      a1[2200] = v3;
      a1[2210] = 0;
      TaskData = PalTaskGetTaskData(0);
      VmSaveState_AdvStack((ScriptContext *)(TaskData + 824));
    }
    sub_430B50();
    return 1;
  }
  if ( a1[201064] )
  {
    PalEndUpdate(10);
    PalDebugPrintf(10, 0, "cancel scene skip select\n");
    v4 = a1[201015];
    a1[201064] = 0;
    v3 = v4 - 12;
    PalDebugPrintf(0, 0, "AdvSetSavePoint");
    goto LABEL_5;
  }
  v6 = a1[201015] - 12;
  PalDebugPrintf(0, 0, "AdvSetSavePoint");
  if ( !a1[2208] && (~(unsigned __int8)VmGetTaskSchedulingState() & 0x10) != 0 )
  {
    a1[2200] = v6;
    a1[2210] = 0;
    v7 = PalTaskGetTaskData(0);
    VmSaveState_AdvStack((ScriptContext *)(v7 + 824));
  }
  v8 = a1[184628];
  if ( (int)(v8 - 1) < 0 )
  {
    PalDebugPrintf(0, 0, "not stack");
    v8 = a1[184628];
  }
  v9 = v8 - 1;
  a1[184628] = v9;
  sub_439C10(a1 + 1, a1[v9 + 177646]);
  v43 = 0;
  v10 = a1 + 165429;
  v39 = 0;
  v40 = a1 + 165509;
  v45 = a1 + 165429;
  while ( 1 )
  {
    v11 = a1[201015];
    v12 = *(_DWORD *)(v11 + a1[201013]);
    v13 = v11 + 4;
    for ( a1[201015] = v13; v12 != 65559; a1[201015] = v13 )
    {
      gOpcodeDispatchTable[(unsigned __int8)v12](a1);
      v14 = a1[201015];
      v12 = *(_DWORD *)(v14 + a1[201013]);
      v13 = v14 + 4;
    }
    v15 = *(_DWORD *)(v13 + a1[201013]);
    a1[201015] = v13 + 8;
    if ( v15 == 393219 )
      break;
    if ( v15 != 393218 )
    {
      a1[165419] = 0;
      v1(0, 0, aDSelect, v13 + 8);
      return 0;
    }
    VmStr_GetFilename(Str1, (int)(a1 + 177637), a1[165421], 1);
    v16 = a1[184628];
    if ( (int)(v16 - 1) < 0 )
    {
      v1(0, 0, "not stack");
      v16 = a1[184628];
    }
    v17 = v16 - 1;
    a1[184628] = v17;
    v36 = a1[v17 + 177646];
    if ( (int)(v17 - 1) < 0 )
    {
      v1(0, 0, "not stack");
      v17 = a1[184628];
    }
    v18 = v17 - 1;
    a1[184628] = v18;
    v38 = a1[v18 + 177646];
    if ( (int)(v18 - 1) < 0 )
    {
      v1(0, 0, "not stack");
      v18 = a1[184628];
    }
    v19 = v18 - 1;
    a1[184628] = v19;
    v46 = a1[v19 + 177646];
    if ( (int)(v19 - 1) < 0 )
    {
      v1(0, 0, "not stack");
      v19 = a1[184628];
    }
    v20 = v19 - 1;
    a1[184628] = v20;
    v42 = a1[v20 + 177646];
    if ( (int)(v20 - 1) < 0 )
    {
      v1(0, 0, "not stack");
      v20 = a1[184628];
    }
    v21 = v20 - 1;
    a1[184628] = v21;
    v41 = a1[v21 + 177646];
    if ( (int)(v21 - 1) < 0 )
    {
      v1(0, 0, "not stack");
      v21 = a1[184628];
    }
    a1[184628] = v21 - 1;
    v37 = a1[v21 + 177645];
    v1(0, 0, "[%s][%d][%d]AdvCommandSelect", Str1, v46, v42);
    v44 = (int)((double)v46 * 1.5);
    v47 = (int)((double)v42 * 1.5);
    v22 = 0;
    if ( v38 )
      v22 = *(_DWORD *)(a1[177644] - 4 * v38 + a1[177640]);
    v10[32] = v22;
    if ( (v41 & 1) != 0 )
    {
      *v10 = sub_447FE0(1080, a1[201058] + 4871);
      v10[48] = v37;
      v10[64] = v41;
      sub_44B120(v51, v36, 0x104u);
      v1(0, 0, "[%d][%d]AdvCommandSelect", v44, v47);
      Active = PalRenderGetActive();
      Text = PalSpriteCreateText(Active);
      v10[16] = Text;
      if ( !Text )
      {
        v1(0, 0, aSelectCreatete, &v51[4]);
        return 0;
      }
      v25 = *v10;
      if ( (v10[64] & 2) != 0 )
      {
        PalSpriteSetColor(v25, a1[165422]);
      }
      else
      {
        PalSpriteSetColor(v25, a1[165422] & 0xFFFFFF | 0x7F000000);
        PalSpriteSetColor(v10[16], 0x7FFFFFFF);
      }
      PalSpriteRectSetPos(*v10, 0, v10[48]);
      ((void (__cdecl *)(_DWORD, _DWORD, _DWORD, _DWORD))PalSpriteSetPos)(*v10, (float)v44, (float)v47, 0);
      PalSpriteGetInfo(*v10, v48);
      v26 = v49 >> 1;
      v27 = v50 >> 1;
      PalSpriteGetInfo(v45[16], v48);
      ((void (__cdecl *)(_DWORD, _DWORD, _DWORD, _DWORD))PalSpriteSetPos)(
        v45[16],
        (float)(v44 + v26 - (v49 >> 1)),
        (float)(v27 - (v50 >> 1) + v47 + 6),
        0);
      v45[1120] = a1[201058] + 4871;
      v35 = *v45;
      v45[1136] = (int)(float)v44;
      v45[1152] = (int)(float)v47;
      PalSpriteViewCtrl(v35, 1);
      PalSpriteViewCtrl(v45[16], 1);
      v28 = 0;
      v29 = 0;
      v30 = 0;
      do
      {
        v31 = v51[v29 + 4];
        if ( !v31 && !v51[v29 + 5] )
          break;
        *((_BYTE *)v40 + v28) = v31;
        v30 += 2;
        v32 = v51[v29 + 5];
        v29 += 2;
        *((_BYTE *)v40 + v28 + 1) = v32;
        v28 += 2;
      }
      while ( v30 < 260 );
      v1 = (void (*)(_DWORD, _DWORD, const char *, ...))PalDebugPrintf;
      *(_WORD *)((char *)v40 + v28) = 0;
      PalDebugPrintf(10, 0, "select[%d] %s, %d, %d", v43, &v51[4], v45[1136], v45[1152]);
      ++v45;
      ++v43;
      v40 += 65;
      v10 = v45;
    }
    if ( ++v39 >= 16 )
    {
      a1[165419] = 0;
      v1(0, 0, aDSelect_0, a1[201015]);
      return 0;
    }
  }
  if ( (VmGetTaskSchedulingState() & 0x30) == 0 )
    a1[201015] -= 12;
  a1[165420] = -1;
  a1[165419] = v43;
  a1[165417] = 1;
  a1[165425] = 0;
  v34 = PalTaskGetTaskData(0);
  *(_DWORD *)((char *)&unk_54D1D4 + v34) = 0;
  *(_DWORD *)((char *)&unk_54D1D0 + v34) = 1;
  *(_DWORD *)((char *)&ctx_off_dbg_flag_b8 + v34) = 1;
  return 1;
}

