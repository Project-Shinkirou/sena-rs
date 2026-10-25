int __cdecl sub_4447B0(int *a1)
{
  int v1; // ecx
  int v2; // ecx
  int v3; // edi
  int v4; // ecx
  int v5; // ecx
  _DWORD *v6; // eax
  int v7; // ecx
  int v8; // eax
  int v9; // ecx
  int v11; // [esp+4h] [ebp-50h]
  int v12; // [esp+8h] [ebp-4Ch]
  int v13; // [esp+Ch] [ebp-48h]
  char Str1[64]; // [esp+10h] [ebp-44h] BYREF

  if ( !sub_442280((int)a1) )
  {
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
    v4 = v2 - 1;
    a1[184628] = v4;
    v11 = a1[v4 + 177646];
    if ( v4 - 1 < 0 )
    {
      PalDebugPrintf(0, 0, "not stack");
      v4 = a1[184628];
    }
    v5 = v4 - 1;
    a1[184628] = v5;
    v13 = a1[v5 + 177646];
    if ( v5 - 1 < 0 )
    {
      PalDebugPrintf(0, 0, "not stack");
      v5 = a1[184628];
    }
    a1[184628] = v5 - 1;
    v12 = a1[v5 + 177645];
    if ( v3 == -1 )
    {
      v3 = 0;
      v6 = a1 + 164268;
      while ( *v6 )
      {
        ++v3;
        v6 += 25;
        if ( v3 >= 8 )
          goto LABEL_16;
      }
      if ( v3 != -1 )
        goto LABEL_17;
LABEL_16:
      v3 = 0;
    }
LABEL_17:
    sub_4426A0((int)(a1 + 163816), v3);
    sub_44B1E0(Str1, v11, 1);
    if ( strlen(Str1) )
    {
      if ( sub_442C10(a1 + 163816, v3, Str1, v13, v12, (v13 & 8) == 0) == 2 )
      {
        v8 = v13;
        if ( (v13 & 2) != 0 )
        {
          if ( (sub_44AC00(v7) & 0x30) != 0 )
          {
            a1[163819] = 0;
          }
          else if ( (sub_44AC00(v9) & 0x30) == 0 )
          {
            a1[201015] -= 12;
          }
          v8 = v13;
        }
        PalDebugPrintf(10, 0, "voice_play  %d, %s, %d, %d", v3, Str1, v8, v12);
      }
      else
      {
        PalDebugPrintf(10, 0, "skip : voice_play  %d, %s, %d, %d", v3, Str1, v13, v12);
      }
      a1[a1[184629] + 177710] = v3;
    }
    else
    {
      PalDebugPrintf(10, 0, "no name : voice_play  %d, %s, %d, %d", v3, Str1, v13, v12);
      a1[a1[184629] + 177710] = 0;
    }
  }
  return 1;
}
