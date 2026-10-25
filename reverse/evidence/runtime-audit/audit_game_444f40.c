// IDA SQL evidence from the local Game.exe.
int __cdecl sub_444F40(_DWORD *a1)
{
  int v1; // ecx
  bool v2; // zf
  int v3; // ecx
  unsigned int v4; // edi
  unsigned int v5; // eax
  int result; // eax
  int v7; // eax
  int v8; // edx
  int v9; // edx
  int v10; // ecx
  int v11; // eax
  int v12; // ecx
  int v13; // ecx

  v2 = a1[163810] == 0;
  a1[201021] = 1;
  if ( !v2 )
  {
    v4 = PaltimeGetTime(v1) - a1[163809];
    if ( a1[163814] && sub_44AC00(v3) )
    {
      a1[163810] = 0;
      v4 = 0;
    }
    v5 = a1[163810];
    if ( v5 != -1 && v4 >= v5 )
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
  if ( !v7 )
  {
    v8 = a1[184628];
    if ( v8 - 1 < 0 )
    {
      PalDebugPrintf(0, 0, "not stack");
      v8 = a1[184628];
    }
    v9 = v8 - 1;
    a1[184628] = v9;
    v10 = a1[v9 + 177646];
    a1[163810] = v10;
    if ( v9 - 1 < 0 )
    {
      PalDebugPrintf(0, 0, "not stack");
      v10 = a1[163810];
      v9 = a1[184628];
    }
    a1[184628] = v9 - 1;
    v11 = a1[v9 + 177645];
    a1[163814] = v11;
    if ( !v10 )
    {
      a1[163810] = 1;
      v10 = 1;
    }
    PalDebugPrintf(10, 0, "wait %d, %d", v10, v11);
    a1[163809] = PaltimeGetTime(v12);
    if ( (sub_44AC00(v13) & 0x30) != 0 )
      a1[163810] = 0;
LABEL_17:
    if ( (sub_44AC00(v3) & 0x30) == 0 )
      a1[201015] -= 12;
  }
  return 1;
}
