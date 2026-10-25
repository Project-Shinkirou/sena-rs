// IDA SQL evidence from the local Game.exe.
int __cdecl sub_444DE0(_DWORD *a1)
{
  int v1; // ecx
  bool v2; // zf
  unsigned int v3; // edi
  int v4; // ecx
  int v5; // ecx
  int result; // eax
  int v7; // eax
  int v8; // ecx
  int v9; // ecx
  int v10; // eax
  int v11; // eax
  int v12; // ecx
  int v13; // [esp-4h] [ebp-Ch]

  v2 = a1[163810] == 0;
  a1[201021] = 1;
  if ( !v2 )
  {
    v3 = PaltimeGetTime(v1) - a1[163809];
    if ( sub_44AC00(v4) || v3 >= a1[163810] )
    {
      a1[163809] = 0;
      result = 1;
      a1[163810] = 0;
      return result;
    }
    goto LABEL_17;
  }
  v7 = a1[163815];
  a1[163815] = 0;
  if ( v7 )
    return 1;
  v8 = a1[184628];
  if ( v8 - 1 < 0 )
  {
    PalDebugPrintf(0, 0, "not stack");
    v8 = a1[184628];
  }
  a1[184628] = v8 - 1;
  v13 = a1[v8 + 177645];
  a1[163810] = v13;
  PalDebugPrintf(10, 0, "wait_click %d", v13);
  v10 = a1[163810];
  if ( v10 != -1 )
  {
    if ( !v10 )
      a1[163810] = 1;
    a1[163809] = PaltimeGetTime(v9);
    if ( (sub_44AC00(v12) & 0x30) != 0 )
      a1[163810] = 0;
LABEL_17:
    if ( (sub_44AC00(v5) & 0x30) == 0 )
      a1[201015] -= 12;
    return 1;
  }
  v11 = a1[1050];
  if ( v11 )
    a1[1050] = v11 & 0xFFFDFFFF;
  else
    sub_43A860((int)(a1 + 1));
  sub_44A010();
  a1[163810] = 0;
  return 1;
}
