// IDA SQL evidence from the local Game.exe.
BOOL __thiscall sub_44A2F0(int *this)
{
  _DWORD *v2; // ebx
  int v3; // eax
  int v4; // ecx
  unsigned int v5; // ecx
  unsigned int v6; // eax
  int v7; // ecx
  int v8; // eax
  unsigned int v9; // edx
  int v10; // ecx
  int v11; // ecx
  int v12; // esi
  int v13; // ecx
  int (__cdecl *v14)(_DWORD); // esi
  int TaskData; // eax
  _DWORD *v16; // esi
  int v17; // ecx
  _DWORD *v18; // eax
  bool v19; // zf
  int v21; // [esp+10h] [ebp-8h]
  int v22; // [esp+14h] [ebp-4h]
  int v23; // [esp+14h] [ebp-4h]

  v2 = (_DWORD *)(PalTaskGetTaskData(0) + 824);
  if ( this[9] )
    return 1;
  this[7] = 1;
  do
  {
    v3 = this[2];
    if ( v3 == this[1] )
      break;
    v4 = this[42];
    if ( v4 )
    {
      if ( v3 == this[v4 + 9] )
        break;
    }
    v5 = v2[201015];
    v2[201056] = v5;
    if ( v2[201066] )
    {
      v6 = v2[201067];
      if ( v6 <= v5 && v6 + 12 > v5 )
      {
        PalListGetDataCount(v2[201057], 2);
        PalDebugPrintf(10, 0, "history_skip end [%x:%x]", v2[201067], v2[201015]);
        sub_439380(v2);
        v22 = v2[201015];
        PalDebugPrintf(0, 0, "AdvSetSavePoint");
        if ( v2[2208] )
        {
          v14 = PalTaskGetTaskData;
        }
        else
        {
          v14 = PalTaskGetTaskData;
          if ( (~(unsigned __int8)sub_44AC00(v13) & 0x10) != 0 )
          {
            v2[2200] = v22;
            v2[2210] = 0;
            TaskData = PalTaskGetTaskData(0);
            sub_42F850((_DWORD *)(TaskData + 824));
          }
        }
        v21 = v2[22083];
        if ( v21 )
        {
          v16 = (_DWORD *)(v14(0) + 824);
          sub_44AC90(v16);
          v16[2211] = v16[2210];
          v16[2210] = 0;
          sub_42EE70((int)v16);
          v16[2207] = 1;
          v16[3240] = PaltimeGetTime(v17);
          v16[3241] = 100;
          v23 = v16[201015];
          v18 = (_DWORD *)PalMemoryAlloc(8, 1);
          if ( v18 )
          {
            *v18 = v23;
            v18[1] = 0;
            PalListPush(v16[201057], v18, 2);
          }
          else
          {
            PalErrorLogPrintf("[%d]memory allocate error DataEntry", v16[201015]);
          }
          v16[201015] = v21 + v16[201016];
        }
        v2[201066] = 0;
        break;
      }
    }
    v7 = this[2];
    v8 = *this;
    this[8] = 0;
    v9 = *(_DWORD *)(v7 + v8);
    v10 = v7 + 4;
    this[2] = v10;
    this[5] = v9;
    if ( HIWORD(v9) == 1 )
    {
      if ( !((int (__cdecl *)(_DWORD *))dword_4BB1D0[(unsigned __int16)v9])(v2) )
        PalDebugPrintf(0, 0, aD_6, this[2], this[5]);
    }
    else
    {
      PalDebugPrintf(0, 0, aD_7, v10, v9);
      PalDebugPrintf(10, 0, aD_7, this[2], this[5]);
    }
    v12 = 16;
    if ( !v2[201064] )
      v12 = 32;
  }
  while ( (sub_44AC00(v11) & v12) != 0 || !this[8] );
  v19 = this[9] == 0;
  this[7] = 0;
  return v19;
}
