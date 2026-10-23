int __fastcall sub_4425F0(int a1, int a2, LPCSTR lpString)
{
  int v3; // ebx
  int v5; // edi
  const CHAR *i; // esi
  size_t v7; // eax
  int v8; // ecx
  int v9; // edx

  v3 = a1 + 4760;
  if ( *(_DWORD *)(a1 + 4764) )
  {
    lstrlenA(lpString);
    v5 = 0;
    for ( i = (const CHAR *)(v3 + 20); ; i += 44 )
    {
      if ( *((_DWORD *)i - 3) )
      {
        v7 = lstrlenA(i);
        if ( !strncmp(i, lpString, v7) )
          break;
      }
      if ( ++v5 >= 32 )
      {
        v8 = 0;
        if ( *(_DWORD *)(v3 + 1420) )
          v8 = 100 * *(_DWORD *)(v3 + 1424);
        PalSoundSetVolumeChannel(a2, v8);
        return 1;
      }
    }
    v9 = 0;
    if ( *(_DWORD *)(44 * v5 + v3 + 12) )
      v9 = 100 * *(_DWORD *)(44 * v5 + v3 + 16);
    PalSoundSetVolumeChannel(a2, v9);
    return 1;
  }
  else
  {
    PalSoundSetVolumeChannel(a2, 10000);
    return 0;
  }
}