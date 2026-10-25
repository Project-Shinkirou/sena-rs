int __cdecl sub_443080(_DWORD *a1)
{
  int v1; // ecx
  int v2; // esi
  int v3; // eax

  v1 = a1[184628];
  if ( v1 - 1 < 0 )
  {
    PalDebugPrintf(0, 0, "not stack");
    v1 = a1[184628];
  }
  a1[184628] = v1 - 1;
  v2 = a1[v1 + 177645];
  v3 = (v2 == 0) * a1[164904];
  a1[164911] = v2 == 0;
  PalSoundSetVolume(v3, 2);
  PalDebugPrintf(10, 0, "bgv_mute %d", v2);
  return 1;
}