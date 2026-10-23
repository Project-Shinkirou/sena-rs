int __cdecl sub_4434E0(_DWORD *a1)
{
  int v1; // edx
  int v2; // edx
  int v3; // ecx
  int v4; // edi
  int v5; // ebx
  _DWORD *v6; // esi
  int v7; // eax
  int v9; // [esp-8h] [ebp-18h]
  int v10; // [esp+Ch] [ebp-4h]

  v1 = a1[184628];
  if ( v1 - 1 < 0 )
  {
    PalDebugPrintf(0, 0, "not stack");
    v1 = a1[184628];
  }
  v2 = v1 - 1;
  a1[184628] = v2;
  v3 = a1[v2 + 177646];
  v10 = v3;
  if ( v2 - 1 < 0 )
  {
    PalDebugPrintf(0, 0, "not stack");
    v2 = a1[184628];
    v3 = v10;
  }
  v4 = 0;
  a1[184628] = v2 - 1;
  v5 = a1[v2 + 177645];
  v6 = a1 + 164493;
  do
  {
    if ( v4 == v3 || v3 == -1 )
    {
      if ( *v6 )
      {
        v7 = 0;
        v6[2] &= ~1u;
        if ( a1[164911] )
          v7 = v5;
        v5 = v7;
        if ( !v7 || sub_44AC00(a1) )
        {
          PalSoundRelease(*v6);
          memset(v6, 0, 0x50u);
        }
        else
        {
          v9 = *v6;
          v6[22] = 0;
          PalSoundStopFade(v9, v5);
        }
      }
      else
      {
        memset(v6, 0, 0x50u);
      }
      v3 = v10;
    }
    ++v4;
    v6 += 31;
  }
  while ( v4 < 8 );
  PalDebugPrintf(10, 0, "bgv_stop  %d, %d", v3, v5);
  return 1;
}