int __cdecl sub_437E70(_DWORD *a1)
{
  int v1; // ecx
  int v2; // ecx
  int v3; // ebx
  int v5; // [esp-4h] [ebp-10h]

  v1 = a1[184628];
  if ( v1 - 1 < 0 )
  {
    PalDebugPrintf(0, 0, "not stack");
    v1 = a1[184628];
  }
  v2 = v1 - 1;
  a1[184628] = v2;
  v3 = a1[v2 + 177646];
  if ( v2 - 1 < 0 )
  {
    PalDebugPrintf(0, 0, "not stack");
    v2 = a1[184628];
  }
  a1[184628] = v2 - 1;
  v5 = a1[v2 + 177645];
  a1[201059] = v3;
  a1[201060] = v5;
  PalInputKeyCancel(v3, v5);
  PalDebugPrintf(10, 0, "key_canncel %d", v3);
  if ( PalDebugIs() && (v3 & 8) != 0 )
    PalDebugPrintf(10, 0, aCtrl);
  return 1;
}