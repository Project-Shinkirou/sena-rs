int __cdecl sub_4382D0(int a1)
{
  int v1; // ecx
  char *v2; // eax
  char v5; // [esp+3h] [ebp-45h] BYREF
  char Str1[64]; // [esp+4h] [ebp-44h] BYREF

  v1 = *(_DWORD *)(a1 + 738512);
  if ( v1 - 1 < 0 )
  {
    PalDebugPrintf(0, 0, "not stack");
    v1 = *(_DWORD *)(a1 + 738512);
  }
  *(_DWORD *)(a1 + 738512) = v1 - 1;
  sub_44B1E0(Str1, *(_DWORD *)(a1 + 4 * (v1 - 1) + 710584), 1);
  v2 = &v5;
  while ( *++v2 )
    ;
  strcpy(v2, ".tga");
  PalExFontUnload();
  PalExFontLoad(Str1);
  PalDebugPrintf(10, 0, "load_font_ex %s", Str1);
  return 1;
}
