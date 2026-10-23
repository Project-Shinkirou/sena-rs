int __fastcall sub_43D770(int a1, int a2, int a3)
{
  int *v5; // ecx
  int v6; // edi
  int v7; // eax
  int *v8; // edi
  int *v9; // esi
  _BYTE *v10; // edi
  signed int v11; // esi
  int v13; // [esp-20h] [ebp-1040h]
  int TaskData; // [esp+10h] [ebp-1010h]
  int v16; // [esp+14h] [ebp-100Ch]
  int v18[1024]; // [esp+1Ch] [ebp-1004h] BYREF

  TaskData = PalTaskGetTaskData(0);
  memset(v18, 0, sizeof(v18));
  v5 = *(int **)(a2 + 4256);
  v6 = *v5;
  if ( (sub_44AC00(v5) & 0x30) == 0 )
    PalSpritePaint(v6, 0, 0);
  v7 = *(_DWORD *)(a2 + 4316);
  v16 = a2 + 4292;
  *(_DWORD *)(a2 + 4292) = 0;
  *(_DWORD *)(a2 + 4312) = v7;
  sub_44B120(v18, a3, 0x1000u);
  PalDebugPrintf(0, 0, "AdvGetTextData  %s", (const char *)v18);
  sub_43C1F0(a1, a2, v18[0]);
  sub_43D350((_WORD *)(a1 + 4608), (CHAR *)&v18[1]);
  v8 = (int *)(a2 + 4240);
  *(_DWORD *)(a2 + 4212) = 0;
  v9 = (int *)(a2 + 4244);
  *v8 = 0;
  v13 = *(_DWORD *)(a2 + 28);
  *v9 = 0;
  PalDebugPrintf(0, 0, "[%d]lpText->tagInfo.lWidth", v13);
  *(_DWORD *)(a2 + 4220) = sub_43C7C0(
                             **(_DWORD **)(a2 + 4256),
                             (unsigned __int8 *)(a1 + 4608),
                             v8,
                             v9,
                             *(_DWORD *)(a2 + 28),
                             *(_DWORD *)(a2 + 32),
                             *(_DWORD *)(a2 + 12),
                             *(_DWORD *)(a2 + 16),
                             (_DWORD *)(a2 + 4328),
                             v16);
  if ( *(_DWORD *)(TaskData + 32) | *(_DWORD *)(a2 + 4192) )
    sub_43A030(**(_DWORD **)(a2 + 4256), (_DWORD *)(a2 + 4292));
  v10 = (_BYTE *)(a2 + 4604);
  if ( !*(_DWORD *)(a1 + 85676) )
  {
    v11 = 0;
    if ( a2 != -4604 )
    {
      while ( v10[v11] || v10[v11 + 1] )
        v11 += 2;
      if ( v11 >= 4096 )
        v11 = 4095;
    }
    PalTaskGetTaskData(0);
    PalDebugPrintf(0, 0, "ADV_HISTORY_TYPE_TEXT");
    memmove_0((void *)(a1 + 88707), v10, v11);
  }
  return PalDebugPrintf(
           0,
           0,
           "[%d][%d][%d][%d][%d][%d][%d][%d][%d][%d]AdvTextSetText",
           *(char *)(a1 + 8860),
           *(char *)(a1 + 8861),
           *(char *)(a1 + 8862),
           *(char *)(a1 + 8863),
           *(char *)(a1 + 8864),
           *(char *)(a1 + 8865),
           *(char *)(a1 + 8866),
           *(char *)(a1 + 8867),
           *(char *)(a1 + 8868),
           *(char *)(a1 + 8869));
}