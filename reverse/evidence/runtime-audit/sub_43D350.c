CHAR __fastcall sub_43D350(_WORD *a1, CHAR *a2)
{
  int TaskData; // eax
  int v5; // ecx
  CHAR result; // al
  int v7; // esi
  int v8; // ecx
  CHAR v9; // al
  int v10; // esi
  int v11; // eax
  int v12; // eax
  CHAR v13; // al
  CHAR *p_i; // ecx
  size_t v15; // esi
  int v16; // [esp+Ch] [ebp-1024h]
  int v17; // [esp+10h] [ebp-1020h]
  int v18; // [esp+14h] [ebp-101Ch]
  CHAR i; // [esp+18h] [ebp-1018h] BYREF
  char v20; // [esp+19h] [ebp-1017h]
  _BYTE v21[4113]; // [esp+1Bh] [ebp-1015h] BYREF

  TaskData = PalTaskGetTaskData(0);
  v5 = 0;
  v16 = TaskData + 824;
  v18 = 0;
  while ( 1 )
  {
    result = *a2;
    if ( !*a2 )
      break;
    if ( result == 110 )
      goto LABEL_4;
    if ( result == 38 )
    {
      if ( a2[1] || (v5 & 1) != 0 )
        goto LABEL_4;
      PalDebugPrintf(0, 0, "[%d][%d]lNumUsed  ", 38, 0);
      *(_DWORD *)&v21[4093] = 1350717314;
      *(_DWORD *)&v21[4097] = 1384272258;
      v7 = 0;
      *(_DWORD *)&v21[4101] = 1417827202;
      *(_DWORD *)&v21[4105] = 1451382146;
      *(_DWORD *)&v21[4109] = 1484937090;
      lstrlenA(a2);
      v9 = *a2;
      for ( i = 0; v9; *(&i + v7) = 0 )
      {
        if ( v9 == 93 )
          break;
        *(&i + v7++) = v9;
        v9 = a2[v7];
      }
      v10 = v7 + 1;
      v11 = sub_4704ED(v8, (int)v21);
      switch ( v20 )
      {
        case 'f':
          v12 = *(_DWORD *)(v16 + 4 * v11 + 732340);
          v17 = v12;
          break;
        case 'm':
          v12 = *(_DWORD *)(v16 + 4 * v11 + 711860);
          v17 = v12;
          break;
        case 't':
          v12 = *(_DWORD *)(v16 + 4 * v11 + 715956);
          v17 = v12;
          break;
        default:
          v12 = v17;
          break;
      }
      if ( v12 < 0 )
      {
        v18 += 2;
        *a1++ = 31873;
        v12 = -v12;
        v17 = v12;
      }
      wsprintfA(&i, "%d", v12);
      v13 = i;
      if ( i )
      {
        p_i = &i;
        do
        {
          p_i += 3;
          *a1++ = *(_WORD *)&v21[2 * v13 + 3997];
          v13 = *p_i;
        }
        while ( *p_i );
      }
      v5 = v18;
      a2 += v10;
    }
    else if ( result != 60 || a2[1] )
    {
LABEL_4:
      ++v5;
      *(_BYTE *)a1 = result;
      v18 = v5;
      a1 = (_WORD *)((char *)a1 + 1);
      ++a2;
    }
    else
    {
      v15 = sub_43BCC0(a2, ">") + 2;
      PalDebugPrintf(0, 0, "[%d]dwTemp  ", v15);
      memmove_0(a1, a2, v15);
      v5 = v18;
      a1 = (_WORD *)((char *)a1 + v15);
      a2 += v15;
    }
  }
  if ( a2[1] )
    goto LABEL_4;
  *a1 = 0;
  return result;
}