int __cdecl sub_4383B0(int a1)
{
  int v1; // ecx
  int v2; // esi
  CHAR pszPath[64]; // [esp+Ch] [ebp-44h] BYREF

  v1 = *(_DWORD *)(a1 + 738512);
  if ( v1 - 1 < 0 )
  {
    PalDebugPrintf(0, 0, "not stack");
    v1 = *(_DWORD *)(a1 + 738512);
  }
  *(_DWORD *)(a1 + 738512) = v1 - 1;
  v2 = *(_DWORD *)(a1 + 4 * (v1 - 1) + 710584);
  PalDebugPrintf(0, 0, "LoadFontFileNo %d", v2);
  sub_44B1E0(pszPath, v2, 1);
  PathAddExtensionA(pszPath, ".dat");
  PalFontLoad(pszPath);
  PalDebugPrintf(10, 0, "load_font %s", pszPath);
  return 1;
}
