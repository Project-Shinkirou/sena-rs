int __cdecl sub_411090(int a1)
{
  int v1; // ecx
  int i; // esi
  char *p_Source; // edx
  int *v4; // esi
  char *j; // ebx
  char *v6; // esi
  unsigned int v7; // edx
  char *v8; // edi
  char *v10; // ecx
  unsigned __int16 v13; // ax
  CHAR *v14; // edx
  CHAR *v15; // esi
  unsigned int v16; // edx
  char *v17; // edi
  char *v19; // edi
  CHAR *v20; // esi
  char v21; // cl
  bool v22; // zf
  int *v23; // [esp+Ch] [ebp-A30h]
  int v24; // [esp+10h] [ebp-A2Ch]
  _BYTE v25[27]; // [esp+14h] [ebp-A28h] BYREF
  char v26; // [esp+2Fh] [ebp-A0Dh] BYREF
  CHAR OutputString[2048]; // [esp+30h] [ebp-A0Ch] BYREF
  char v28[4]; // [esp+830h] [ebp-20Ch] BYREF
  char Source; // [esp+834h] [ebp-208h] BYREF
  CHAR Str1[4]; // [esp+934h] [ebp-108h] BYREF
  char v31; // [esp+938h] [ebp-104h] BYREF

  memset(OutputString, 0, sizeof(OutputString));
  v1 = *(_DWORD *)(a1 + 738512);
  for ( i = 0; i < 8; ++i )
  {
    if ( v1 - 1 < 0 )
    {
      PalDebugPrintf(0, 0, "not stack");
      v1 = *(_DWORD *)(a1 + 738512);
    }
    *(_DWORD *)(a1 + 738512) = --v1;
    *(_DWORD *)&v25[4 * i - 4] = *(_DWORD *)(a1 + 4 * v1 + 710584);
  }
  if ( !PalDebugIs() )
    return 1;
  sub_44B120(v28, v24, 0x104u);
  p_Source = &Source;
  if ( !Source )
    goto LABEL_14;
  v4 = (int *)v25;
  v23 = (int *)v25;
  while ( 2 )
  {
    for ( j = p_Source; !*j; j += 2 )
    {
      if ( !j[1] )
      {
        v6 = p_Source;
        v7 = strlen(p_Source) + 1;
        v8 = &v26;
        while ( *++v8 )
          ;
        qmemcpy(v8, v6, v7);
        goto LABEL_14;
      }
LABEL_20:
      ;
    }
    if ( *j != 37 || j[1] )
      goto LABEL_20;
    strncat(OutputString, p_Source, j - p_Source);
    v13 = *(_WORD *)j;
    if ( *(_WORD *)j > 0x6425u )
    {
      if ( v13 == 26149 )
      {
LABEL_27:
        sub_44B1E0(Str1, *v4, 0);
LABEL_28:
        v14 = Str1;
LABEL_29:
        v23 = v4 + 1;
        goto LABEL_30;
      }
      if ( v13 == 29733 )
      {
LABEL_42:
        sub_44B120(Str1, *v4, 0x104u);
        v14 = &v31;
        goto LABEL_29;
      }
      v22 = *(_WORD *)j == 30757;
LABEL_39:
      if ( !v22 )
        goto LABEL_40;
      wsprintfA(Str1, "%08X", *v4);
      goto LABEL_28;
    }
    if ( v13 == 25637 )
    {
LABEL_35:
      wsprintfA(Str1, "%d", *v4);
      goto LABEL_28;
    }
    if ( v13 > 0x5425u )
    {
      v22 = v13 == 22565;
      goto LABEL_39;
    }
    switch ( v13 )
    {
      case 0x5425u:
        goto LABEL_42;
      case 0x4425u:
        goto LABEL_35;
      case 0x4625u:
        goto LABEL_27;
    }
LABEL_40:
    v14 = (CHAR *)Default;
LABEL_30:
    v15 = v14;
    v16 = strlen(v14) + 1;
    v17 = &v26;
    while ( *++v17 )
      ;
    qmemcpy(v17, v15, 4 * (v16 >> 2));
    v20 = &v15[4 * (v16 >> 2)];
    v19 = &v17[4 * (v16 >> 2)];
    v21 = v16;
    p_Source = j + 2;
    qmemcpy(v19, v20, v21 & 3);
    if ( j[2] )
    {
      v4 = v23;
      continue;
    }
    break;
  }
LABEL_14:
  PalDebugPrintf(16, 0, OutputString);
  v10 = &v26;
  while ( *++v10 )
    ;
  strcpy(v10, "\r\n");
  OutputDebugStringA(OutputString);
  return 1;
}