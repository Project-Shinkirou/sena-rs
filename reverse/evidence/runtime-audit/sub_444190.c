int __cdecl sub_444190(int a1)
{
  int v1; // ecx

  v1 = *(_DWORD *)(a1 + 738512);
  if ( v1 - 1 < 0 )
  {
    PalDebugPrintf(0, 0, "not stack");
    v1 = *(_DWORD *)(a1 + 738512);
  }
  *(_DWORD *)(a1 + 738512) = v1 - 1;
  *(_DWORD *)(a1 + 660028) = *(_DWORD *)(a1 + 4 * (v1 - 1) + 710584);
  return 1;
}
