// Fresh IDA evidence from testcase/dll/Pal.dll, analyzed from a copy.
int __cdecl PalEffectEx(unsigned int a1, int a2, int a3, int a4)
{
  int v5; // [esp+0h] [ebp-8h]
  int v6; // [esp+4h] [ebp-4h]

  if ( !*((_DWORD *)dword_101294A4 + 191) )
    return 1;
  if ( !a1 )
    return 1;
  if ( a1 > 0x1D )
    return 0;
  if ( *((_DWORD *)dword_101294A4 + 190) )
    return 0;
  if ( Pal3DIs() )
  {
    v6 = 10;
    do
      v5 = v6--;
    while ( v5 && !sub_100A9B50(*(_DWORD *)dword_101294A4, *(_DWORD *)(*((_DWORD *)dword_101294A4 + 205) + 8)) );
  }
  else
  {
    sub_100A4B90(*((_DWORD *)dword_101294A4 + 1), *(_DWORD *)(*((_DWORD *)dword_101294A4 + 204) + 8));
  }
  *((_DWORD *)dword_101294A4 + 194) = std::_Get_future_error_what(a1);
  *((_DWORD *)dword_101294A4 + 198) = a1;
  *((_DWORD *)dword_101294A4 + 190) = 1;
  *((_DWORD *)dword_101294A4 + 192) = a3;
  *((_DWORD *)dword_101294A4 + 196) = a2;
  PalUpdateTime();
  if ( a4 )
    *((_DWORD *)dword_101294A4 + 193) = PalWait(-1);
  return 1;
}

