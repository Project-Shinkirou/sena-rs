// IDA database Game.sqlite, reference Game.exe, address 0x411760.
// Build identity: reverse/evidence/native-interface-inventory.json.
int __cdecl sub_411760(_DWORD *a1)
{
  int v1; // ecx
  int v2; // ecx
  int v3; // ecx
  int v4; // ecx
  unsigned int v5; // eax
  float v6; // xmm0_4
  int v8; // [esp+8h] [ebp-14h] BYREF
  _DWORD v9[3]; // [esp+Ch] [ebp-10h] BYREF

  v1 = a1[184628];
  if ( v1 - 1 < 0 )
  {
    PalDebugPrintf(0, 0, "not stack");
    v1 = a1[184628];
  }
  a1[184628] = v1 - 1;
  a1[204546] = a1[v1 + 177645];
  v2 = a1[184628];
  if ( v2 - 1 < 0 )
  {
    PalDebugPrintf(0, 0, "not stack");
    v2 = a1[184628];
  }
  a1[184628] = v2 - 1;
  a1[204547] = a1[v2 + 177645];
  v3 = a1[184628];
  if ( v3 - 1 < 0 )
  {
    PalDebugPrintf(0, 0, "not stack");
    v3 = a1[184628];
  }
  a1[184628] = v3 - 1;
  a1[204548] = a1[v3 + 177645];
  v4 = a1[184628];
  if ( v4 - 1 < 0 )
  {
    PalDebugPrintf(0, 0, "not stack");
    v4 = a1[184628];
  }
  a1[184628] = v4 - 1;
  a1[204549] = a1[v4 + 177645];
  a1[201081] |= 0x10u;
  a1[204545] = 1;
  v8 = a1[201058] - a1[204546] + 4999;
  v5 = a1[204548];
  v6 = (float)(unsigned int)a1[204547];
  *(float *)&v9[1] = v6 * 0.0099999998;
  v9[0] = a1[204549];
  *(float *)&v9[2] = (float)v5 * 0.0099999998;
  PalSetFxEffect(&v8, 0x10000000, 1, v9[0], v9);
  PalDebugPrintf(10, 0, "raster %d, %d, %d, %d", a1[204546], a1[204547], a1[204548], a1[204549]);
  a1[201021] = 1;
  return 1;
}
