// IDA evidence from the local PAL.dll at 0x100a8140.
int __cdecl PalEffectEx(unsigned int a1, int a2, int a3, int a4)
{
  _DWORD *v4; // ecx
  int v5; // edi
  unsigned int *v6; // esi
  unsigned int v7; // ebx
  int v8; // esi
  _DWORD *v9; // ecx
  char *v10; // eax
  char v12[4]; // [esp+0h] [ebp-Ch] BYREF
  unsigned int *v13; // [esp+4h] [ebp-8h]
  _DWORD *v14; // [esp+8h] [ebp-4h] BYREF

  v4 = dword_10128698;
  if ( !*((_DWORD *)dword_10128698 + 277) || !a1 )
    return 1;
  if ( a1 <= 0x31 && !*((_DWORD *)dword_10128698 + 276) )
  {
    if ( (*((_BYTE *)dword_10128698 + 1060) & 8) != 0 )
    {
      v5 = 10;
      while ( 1 )
      {
        --v5;
        v6 = *(unsigned int **)(v4[291] + 8);
        v14 = (_DWORD *)*v4;
        v7 = *v6;
        if ( !(*(int (__stdcall **)(unsigned int, _DWORD, char *, _DWORD, int))(*(_DWORD *)*v6 + 76))(
                *v6,
                0,
                v12,
                0,
                0x2000) )
        {
          v8 = sub_100C47C0((int)v14, v13, v6[1], v6[2]);
          (*(void (__stdcall **)(unsigned int, _DWORD))(*(_DWORD *)v7 + 80))(v7, 0);
          if ( v8 )
            break;
        }
        if ( !v5 )
          break;
        v4 = dword_10128698;
      }
    }
    else
    {
      sub_100BEE70(
        *(_DWORD **)(**(_DWORD **)(*((_DWORD *)dword_10128698 + 290) + 8) + 8),
        *(unsigned int **)(**((_DWORD **)dword_10128698 + 1) + 16),
        *(_DWORD *)(*((_DWORD *)dword_10128698 + 1) + 36),
        *(_DWORD *)(*((_DWORD *)dword_10128698 + 1) + 40));
    }
    v9 = dword_10128698;
    *((_DWORD *)dword_10128698 + 284) = a1;
    v9[280] = dword_1010C5D0[a1];
    v9[278] = a3;
    v9[276] = 1;
    v9[282] = a2;
    *((_DWORD *)dword_10128698 + 3184) = timeGetTime();
    if ( a4 )
    {
      v10 = PalTaskCreate((int)sub_100BAE60, 0, 4u, &v14, 65537, 0, (int)"PalWait");
      if ( v14 )
        *v14 = -1;
      *((_DWORD *)dword_10128698 + 279) = v10;
    }
    return 1;
  }
  return 0;
}

