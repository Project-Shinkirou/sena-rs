// IDA evidence from the local PAL.dll at 0x100b1fe0.
int __cdecl PalSpriteSetFxEffect(int a1, int a2, int a3, int a4, void *a5)
{
  _DWORD *v5; // esi
  int result; // eax
  _DWORD *v7; // eax
  void *v8; // ecx
  void *v9; // ecx

  v5 = *(_DWORD **)(a1 + 28);
  if ( !v5 )
  {
    v5 = sub_100A8880();
    *(_DWORD *)(a1 + 28) = v5;
    if ( !v5 )
    {
      SetLastError(6u);
      return 0;
    }
  }
  if ( a3 )
  {
    if ( a3 == 1 )
    {
      if ( v5[3] || !sub_100A8630(v5, a2, a4, a5) )
        return 0;
    }
    else if ( a3 == 2 )
    {
      v5[2] = a4;
      v7 = dword_10128698;
      v5[3] = 2;
      v5[1] = v7[3184];
      result = 1;
      *v5 = 1;
      return result;
    }
  }
  else
  {
    v8 = (void *)v5[6];
    v5[3] = 0;
    if ( v8 )
    {
      if ( dword_10128698 )
        --*((_DWORD *)dword_10128698 + 84);
      j___free_base(v8);
      v5[6] = 0;
    }
    v9 = (void *)v5[5];
    if ( v9 )
    {
      if ( dword_10128698 )
        --*((_DWORD *)dword_10128698 + 84);
      j___free_base(v9);
      v5[5] = 0;
    }
  }
  return 1;
}

