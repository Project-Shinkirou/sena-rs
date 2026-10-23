int __cdecl sub_4435F0(_DWORD *a1)
{
  int v1; // ecx
  int v2; // ecx
  int v3; // esi
  int v4; // ecx
  int v5; // ecx
  _DWORD *v6; // eax
  int *v7; // ebx
  _DWORD *v8; // ebx
  int v9; // ecx
  int v10; // eax
  int v11; // ebx
  bool v12; // zf
  int v14; // [esp+Ch] [ebp-58h]
  int v15; // [esp+10h] [ebp-54h]
  int *v16; // [esp+14h] [ebp-50h]
  int v17; // [esp+18h] [ebp-4Ch]
  int v18; // [esp+1Ch] [ebp-48h]
  char Str1[64]; // [esp+20h] [ebp-44h] BYREF

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
  v14 = a1[v4 + 177646];
  if ( v4 - 1 < 0 )
  {
    PalDebugPrintf(0, 0, "not stack");
    v4 = a1[184628];
  }
  v5 = v4 - 1;
  a1[184628] = v5;
  v18 = a1[v5 + 177646];
  if ( v5 - 1 < 0 )
  {
    PalDebugPrintf(0, 0, "not stack");
    v5 = a1[184628];
  }
  a1[184628] = v5 - 1;
  v15 = a1[v5 + 177645];
  if ( v3 == -1 )
  {
    v3 = 0;
    v6 = a1 + 164493;
    while ( *v6 )
    {
      ++v3;
      v6 += 31;
      if ( v3 >= 8 )
        goto LABEL_15;
    }
    if ( v3 != -1 )
      goto LABEL_16;
LABEL_15:
    v3 = 0;
  }
LABEL_16:
  v7 = &a1[31 * v3 + 164493];
  v16 = v7;
  if ( *v7 && v3 < v3 + 1 )
  {
    v8 = &a1[31 * v3 + 164493];
    v9 = 1;
    v17 = 1;
    do
    {
      if ( *v8 )
      {
        PalSoundRelease(*v8);
        memset(v8, 0, 0x7Cu);
        v9 = v17;
      }
      v8 += 31;
      v17 = --v9;
    }
    while ( v9 );
    v7 = v16;
  }
  v7[1] = v14;
  v7[2] = v18;
  sub_44B1E0(Str1, v14, 1);
  v10 = sub_4024D0(Str1, 2, 0);
  *v7 = v10;
  if ( v10 )
  {
    v11 = v18 & 1;
    if ( (v18 & 4) != 0 && (v12 = (sub_44AC00(v18) & 0x35) == 0, v10 = *v16, v12) )
      PalSoundPlayFade(v10, v11, v15);
    else
      PalSoundPlay(v10, v11);
  }
  a1[a1[184629] + 177710] = v3;
  PalDebugPrintf(10, 0, "bgv_play %d, %s, %d, %d", v3, Str1, v18, v15);
  return 1;
}