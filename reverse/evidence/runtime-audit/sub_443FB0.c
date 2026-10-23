int __cdecl sub_443FB0(_DWORD *a1)
{
  int v1; // ecx
  int v2; // ecx
  int v3; // esi
  int v4; // ecx
  int v5; // ecx
  int v6; // edx
  int v7; // ecx
  char *v8; // esi
  char *v9; // ecx
  int v10; // esi
  char v11; // al
  int v12; // eax
  size_t v13; // eax
  int v15; // [esp+Ch] [ebp-54h]
  int v16; // [esp+10h] [ebp-50h]
  int v17; // [esp+14h] [ebp-4Ch]
  int v18; // [esp+18h] [ebp-48h]
  char Str1[64]; // [esp+1Ch] [ebp-44h] BYREF

  memset(Str1, 0, sizeof(Str1));
  v1 = a1[184628];
  if ( v1 - 1 < 0 )
  {
    PalDebugPrintf(0, 0, "not stack");
    v1 = a1[184628];
  }
  v2 = v1 - 1;
  a1[184628] = v2;
  v3 = a1[v2 + 177646];
  v15 = v3;
  if ( v2 - 1 < 0 )
  {
    PalDebugPrintf(0, 0, "not stack");
    v2 = a1[184628];
  }
  v4 = v2 - 1;
  a1[184628] = v4;
  v18 = a1[v4 + 177646];
  if ( v4 - 1 < 0 )
  {
    PalDebugPrintf(0, 0, "not stack");
    v4 = a1[184628];
  }
  v5 = v4 - 1;
  a1[184628] = v5;
  v6 = a1[v5 + 177646];
  v17 = v6;
  if ( v5 - 1 < 0 )
  {
    PalDebugPrintf(0, 0, "not stack");
    v5 = a1[184628];
    v6 = v17;
  }
  a1[184628] = v5 - 1;
  v7 = a1[v5 + 177645];
  v16 = v7;
  if ( v3 != -1 )
  {
    v12 = 11 * v3;
    v8 = (char *)&a1[11 * v3 + 165011];
    a1[v12 + 165008] = 1;
    a1[v12 + 165010] = v6;
    a1[v12 + 165009] = v7;
    if ( v18 != -1 )
    {
      sub_44B1E0(Str1, v18, 1);
      strcpy(v8, Str1);
      goto LABEL_17;
    }
    goto LABEL_15;
  }
  v8 = (char *)(a1 + 165363);
  a1[165360] = 1;
  a1[165361] = v7;
  a1[165362] = v6;
  if ( v18 == -1 )
  {
LABEL_15:
    v13 = lstrlenA(v8);
    memmove_0(Str1, v8, v13);
    goto LABEL_17;
  }
  sub_44B1E0(Str1, v18, 1);
  v9 = Str1;
  v10 = v8 - Str1;
  do
  {
    v11 = *v9++;
    v9[v10 - 1] = v11;
  }
  while ( v11 );
LABEL_17:
  PalDebugPrintf(10, 0, "set_voice_info  %d, %s, %d, %d", v15, Str1, v17, v16);
  return 1;
}