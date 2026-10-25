int __cdecl sub_443800(_DWORD *a1)
{
  int v1; // ecx
  int v2; // edi
  int v3; // ecx
  int v4; // eax
  int v5; // eax
  int v6; // ecx

  if ( !sub_442280((int)a1) )
  {
    v1 = a1[184628];
    if ( v1 - 1 < 0 )
    {
      PalDebugPrintf(0, 0, "not stack");
      v1 = a1[184628];
    }
    a1[184628] = v1 - 1;
    v2 = a1[v1 + 177645];
    if ( (sub_44AC00(v1) & 0x30) == 0 )
    {
      v4 = a1[163819];
      if ( v2 == -1 )
        v5 = v4 | 0xFF;
      else
        v5 = v4 | (1 << v2);
      a1[163819] = v5;
      if ( (sub_44AC00(v3) & 0x30) == 0 )
        a1[201015] -= 12;
      a1[3240] = PaltimeGetTime(v6);
      a1[3241] = -1;
    }
    PalDebugPrintf(10, 0, "voice_wait %d", v2);
  }
  return 1;
}
