int __fastcall sub_442550(char *String2, int a2, int a3)
{
  int v4; // edi
  const CHAR *i; // esi
  size_t v6; // eax
  int result; // eax
  _DWORD *v8; // ecx

  *(_OWORD *)a2 = 0LL;
  lstrlenA(String2);
  v4 = 0;
  for ( i = (const CHAR *)(a3 + 8); ; i += 40 )
  {
    if ( *i )
    {
      v6 = lstrlenA(i);
      if ( !_strnicmp(i, String2, v6) )
        break;
    }
    if ( ++v4 >= 16 )
      return 0;
  }
  v8 = (_DWORD *)(a3 + 40 * v4);
  *(_DWORD *)a2 = *v8;
  *(_DWORD *)(a2 + 4) = v8[1];
  result = 1;
  *(_DWORD *)(a2 + 12) = 2;
  return result;
}
