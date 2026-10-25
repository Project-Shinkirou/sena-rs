int __fastcall sub_4024D0(const char *a1, int a2, int a3)
{
  const char *v4; // eax
  int v5; // edi
  int v6; // esi
  int result; // eax
  CHAR v9[260]; // [esp+14h] [ebp-108h] BYREF

  if ( (sub_44AC00() & 0x30) != 0 )
    return 0;
  v4 = ".ogg";
  v5 = 0;
  if ( !".ogg" )
  {
LABEL_7:
    if ( a2 != 1 )
      PalDebugPrintf(0, 0, "sound load error [%s]", a1);
    return 0;
  }
  v6 = 0;
  while ( 1 )
  {
    wsprintfA(v9, "%s%s", a1, v4);
    result = PalSoundLoad(v9, a2, a3);
    if ( result )
      return result;
    if ( (&off_49BE44)[v6] )
    {
      v6 = ++v5;
      v4 = off_49BE40[v5];
      if ( v4 )
        continue;
    }
    goto LABEL_7;
  }
}