int __cdecl sub_443150(_DWORD *a1)
{
  int v1; // ecx
  int v2; // esi

  v1 = a1[184628];
  if ( v1 - 1 < 0 )
  {
    PalDebugPrintf(0, 0, "not stack");
    v1 = a1[184628];
  }
  a1[184628] = v1 - 1;
  v2 = 100 * a1[v1 + 177645];
  PalSoundSetVolume(v2 * a1[164911], 2);
  a1[164904] = v2;
  PalDebugPrintf(10, 0, "set_bgv_volume %d", v2);
  return 1;
}