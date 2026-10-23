// IDA database Game.sqlite, reference Game.exe, address 0x422870.
// Build identity: reverse/evidence/native-interface-inventory.json.
int __cdecl sub_422870(int a1)
{
  int v2; // ecx
  int v3; // ecx
  int v4; // ebx
  int v5; // ecx
  int v7; // [esp+14h] [ebp+8h]

  v2 = *(_DWORD *)(a1 + 738512);
  if ( v2 - 1 < 0 )
  {
    PalDebugPrintf(0, 0, "not stack");
    v2 = *(_DWORD *)(a1 + 738512);
  }
  v3 = v2 - 1;
  *(_DWORD *)(a1 + 738512) = v3;
  v7 = *(_DWORD *)(a1 + 4 * v3 + 710584);
  if ( v3 - 1 < 0 )
  {
    PalDebugPrintf(0, 0, "not stack");
    v3 = *(_DWORD *)(a1 + 738512);
  }
  *(_DWORD *)(a1 + 738512) = v3 - 1;
  v4 = *(_DWORD *)(a1 + 4 * (v3 - 1) + 710584);
  v5 = *(_DWORD *)(344 * v7 + a1 + 666392);
  if ( v5 )
  {
    *(_DWORD *)(v5 + 4) = 2;
    PalSetFxEffect(*(_DWORD *)(344 * v7 + a1 + 666388), 0, 2, v4, 0);
  }
  PalDebugPrintf(10, 0, "fx_raster_stop %d, %d", v7, v4);
  return 1;
}
