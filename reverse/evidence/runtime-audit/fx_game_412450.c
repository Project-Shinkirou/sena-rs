// IDA database Game.sqlite, reference Game.exe, address 0x412450.
// Build identity: reverse/evidence/native-interface-inventory.json.
int __cdecl sub_412450(_DWORD *a1)
{
  void (*v1)(_DWORD, _DWORD, const char *, ...); // ecx
  int v2; // edx
  int v3; // edx
  int v4; // ebx
  int v5; // edi
  int v6; // ecx

  v1 = (void (*)(_DWORD, _DWORD, const char *, ...))PalDebugPrintf;
  v2 = a1[184628];
  if ( v2 - 1 < 0 )
  {
    PalDebugPrintf(0, 0, "not stack");
    v2 = a1[184628];
    v1 = (void (*)(_DWORD, _DWORD, const char *, ...))PalDebugPrintf;
  }
  v3 = v2 - 1;
  a1[184628] = v3;
  v4 = a1[v3 + 177646];
  if ( v3 - 1 < 0 )
  {
    v1(0, 0, "not stack");
    v3 = a1[184628];
  }
  a1[184628] = v3 - 1;
  v5 = a1[v3 + 177645];
  if ( sub_44AC00(v1) )
  {
    v5 = 0;
    PalDebugPrintf(10, 0, "effect_stop skip");
  }
  else
  {
    PalDebugPrintf(10, 0, "effect_stop %d, %d", v4, v5);
  }
  if ( (v4 & 1) != 0 )
  {
    if ( (a1[201080] & 2) != 0 )
    {
      sub_449FC0();
      a1[201080] &= ~2u;
    }
    if ( a1[201082] == 1 )
    {
      a1[201082] = 2;
      a1[201083] = PaltimeGetTime(v6);
      a1[201084] = v5;
      if ( (a1[201080] & 1) != 0 )
      {
        sub_449FC0();
        a1[201080] &= ~1u;
      }
    }
  }
  if ( (v4 & 4) != 0 && a1[203133] == 1 )
  {
    a1[203133] = 2;
    a1[203136] = v5;
    a1[203135] = PaltimeGetTime(v6);
  }
  if ( (v4 & 8) != 0 && a1[204538] == 1 )
  {
    a1[204538] = 2;
    a1[204540] = v5;
    a1[204539] = PaltimeGetTime(v6);
  }
  if ( (v4 & 0x10) != 0 && a1[204545] == 1 )
  {
    a1[204545] = 2;
    PalSetFxEffect(a1 + 204546, 0x10000000, 2, v5, 0);
  }
  return 1;
}
