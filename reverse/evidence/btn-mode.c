// Game.exe 0x410BA0; exported from the read-only Game.sqlite decompilation.
int __cdecl sub_410BA0(int a1)
{
  int v1; // ecx
  int v2; // eax
  int v3; // esi

  v1 = *(_DWORD *)(a1 + 738512);
  if ( v1 - 1 < 0 )
  {
    PalDebugPrintf(0, 0, "not stack");
    v1 = *(_DWORD *)(a1 + 738512);
  }
  v2 = v1 - 1;
  *(_DWORD *)(a1 + 738512) = v1 - 1;
  v3 = *(_DWORD *)(a1 + 4 * (v1 - 1) + 710584);
  LOBYTE(v2) = v3 != 0;
  *(_BYTE *)(a1 + 12956) = v3 != 0;
  PalButtonMode(v2);
  PalDebugPrintf(10, 0, "btn_mode  %d", v3);
  return 1;
}
