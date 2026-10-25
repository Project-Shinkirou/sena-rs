// IDA evidence from the local PAL.dll at 0x100a8b90.
int __cdecl PalSetFxEffect(int *a1, int a2, int a3, int a4, void *a5)
{
  _DWORD *v6; // esi
  _DWORD *v7; // eax
  int result; // eax
  void *v9; // ecx
  void *v10; // ecx
  int v11; // [esp-4h] [ebp-10h]
  int v12; // [esp+18h] [ebp+Ch]

  if ( (a2 & 0x10000000) != 0 )
  {
    v6 = (_DWORD *)&xmmword_1012869C + 1;
    if ( a3 == 1 )
    {
      if ( (_DWORD)xmmword_1012869C )
        return 0;
      v11 = *a1;
      LODWORD(xmmword_1012869C) = *((_DWORD *)dword_10128698 + 3186);
      PalRenderInsert(xmmword_1012869C, (int)&xmmword_1012869C + 4, (int)sub_100A8940, v11);
    }
  }
  else
  {
    v12 = *a1;
    v6 = *(_DWORD **)(*a1 + 28);
    if ( !v6 )
    {
      v6 = sub_100A8880();
      *(_DWORD *)(v12 + 28) = v6;
      if ( !v6 )
      {
        SetLastError(6u);
        return 0;
      }
    }
  }
  if ( a3 )
  {
    if ( a3 != 1 )
    {
      if ( a3 == 2 )
      {
        v6[2] = a4;
        v7 = dword_10128698;
        v6[3] = 2;
        v6[1] = v7[3184];
        result = 1;
        *v6 = 1;
        return result;
      }
      return 1;
    }
    if ( v6[3] || !sub_100A8630(v6, a2 & 0xFFFFFFF, a4, a5) )
      return 0;
  }
  else
  {
    v9 = (void *)v6[6];
    v6[3] = 0;
    if ( v9 )
    {
      if ( dword_10128698 )
        --*((_DWORD *)dword_10128698 + 84);
      j___free_base(v9);
      v6[6] = 0;
    }
    v10 = (void *)v6[5];
    if ( v10 )
    {
      if ( dword_10128698 )
        --*((_DWORD *)dword_10128698 + 84);
      j___free_base(v10);
      v6[5] = 0;
    }
  }
  return 1;
}

