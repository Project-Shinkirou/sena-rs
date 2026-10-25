int __fastcall sub_44E370(LPCSTR lpString, _DWORD *a2)
{
  int v2; // ebx
  int v3; // esi
  CHAR *v4; // eax
  CHAR *v5; // edi
  CHAR v6; // cl
  CHAR *i; // edx
  int j; // ecx
  char v9; // al
  int v10; // edx
  CHAR v11; // cl
  char v12; // al
  char v13; // cl
  int v17; // [esp+14h] [ebp-4h]

  v17 = 0;
  v2 = lstrlenA(lpString);
  v3 = -1;
  v4 = (CHAR *)malloc(v2 + 1);
  v5 = v4;
  if ( !v4 )
    return 0;
  wsprintfA(v4, "%s", lpString);
  v6 = *v5;
  for ( i = v5; v6; ++i )
  {
    if ( (unsigned __int8)(v6 - 65) <= 0x19u )
      *i = v6 + 32;
    v6 = i[1];
  }
  for ( j = 0; j < v2; ++v3 )
  {
    v9 = v5[j];
    if ( (v9 < 48 || v9 > 57) && (v9 < 97 || v9 > 102) && v9 != 120 )
      break;
    ++j;
  }
  if ( a2 )
    *a2 = v3 + 1;
  if ( v3 > -1 )
  {
    v10 = 0;
    do
    {
      v11 = v5[v3];
      if ( v11 == 120 )
        break;
      --v3;
      v12 = v11 - 87 + ((unsigned __int8)(v11 - 97) > 5u ? 0x27 : 0);
      v13 = v10;
      v10 += 4;
      v17 += v12 << v13;
    }
    while ( v3 > -1 );
  }
  j___free_base(v5);
  return v17;
}
