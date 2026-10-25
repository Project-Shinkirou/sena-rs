// IDA database Game.sqlite, reference Game.exe, address 0x422790.
// Build identity: reverse/evidence/native-interface-inventory.json.
int __cdecl sub_422790(_DWORD *a1)
{
  int v1; // ecx
  int v2; // eax
  int v3; // edi
  int v4; // ecx
  int v6; // [esp-4h] [ebp-Ch]

  v1 = a1[184628];
  if ( v1 - 1 < 0 )
  {
    PalDebugPrintf(0, 0, "not stack");
    v1 = a1[184628];
  }
  a1[184628] = v1 - 1;
  v2 = a1[v1 + 177645];
  if ( !dword_4BD6F0 )
  {
    v6 = a1[v1 + 177645];
    dword_4BD6F0 = 1;
    dword_4BD6F4 = v2;
    PalDebugPrintf(10, 0, "fx_effect_wait %d", v6);
  }
  v3 = 86 * dword_4BD6F4;
  if ( !a1[86 * dword_4BD6F4 + 166597] )
    return 1;
  if ( sub_44AC00(v1) || !PalGetFxEffectState(a1[v3 + 166597], 0xFFFFFFF) )
  {
    dword_4BD6F0 = 0;
    return 1;
  }
  if ( (sub_44AC00(v4) & 0x30) == 0 )
    a1[201015] -= 12;
  a1[201021] = 1;
  return 1;
}
