int __cdecl sub_443B60(int a1)
{
  int v1; // ecx
  int v3; // [esp-4h] [ebp-8h]

  v1 = *(_DWORD *)(a1 + 738512);
  if ( v1 - 1 < 0 )
  {
    PalDebugPrintf(0, 0, "not stack");
    v1 = *(_DWORD *)(a1 + 738512);
  }
  *(_DWORD *)(a1 + 738512) = v1 - 1;
  v3 = *(_DWORD *)(a1 + 4 * (v1 - 1) + 710584);
  *(_DWORD *)(a1 + 661488) = 100 * v3;
  PalDebugPrintf(10, 0, "voice_autopan_initialize %d", v3);
  return 1;
}