int __cdecl sub_432620(int a1)
{
  int v1; // ecx
  int v2; // eax
  int v3; // edi
  int v4; // esi
  int v5; // ecx
  int v7; // [esp+Ch] [ebp-10Ch]
  CHAR pszPath[260]; // [esp+10h] [ebp-108h] BYREF

  v1 = *(_DWORD *)(a1 + 738512);
  if ( v1 - 1 < 0 )
  {
    PalDebugPrintf(0, 0, "not stack");
    v1 = *(_DWORD *)(a1 + 738512);
  }
  *(_DWORD *)(a1 + 738512) = v1 - 1;
  v2 = *(_DWORD *)(a1 + 4 * (v1 - 1) + 710584);
  v7 = v2;
  if ( v2 == -1 )
  {
    v3 = 1000;
    v4 = 0;
    goto LABEL_6;
  }
  v3 = v2 + 1;
  v4 = *(_DWORD *)(a1 + 4 * (v1 - 1) + 710584);
  v5 = 0;
  if ( !__OFSUB__(v2, v2 + 1) )
  {
LABEL_6:
    while ( 1 )
    {
      wsprintfA(pszPath, "%s\\save%03d.dat", DirectoryName, v4);
      if ( PathFileExistsA(pszPath) )
        break;
      if ( ++v4 >= v3 )
      {
        v5 = 0;
        goto LABEL_10;
      }
    }
    v5 = 1;
  }
LABEL_10:
  *(_DWORD *)(a1 + 4 * *(_DWORD *)(a1 + 738516) + 710840) = v5;
  PalDebugPrintf(10, 0, "[%d] is_save %d", v5, v7);
  return 1;
}
