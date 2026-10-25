int __cdecl sub_441F80(int a1)
{
  int v1; // ecx
  int v2; // ecx
  int v3; // ebx
  int v4; // ecx
  unsigned __int8 v5; // al

  v1 = *(_DWORD *)(a1 + 738512);
  if ( v1 - 1 < 0 )
  {
    PalDebugPrintf(0, 0, "not stack");
    v1 = *(_DWORD *)(a1 + 738512);
  }
  v2 = v1 - 1;
  *(_DWORD *)(a1 + 738512) = v2;
  v3 = *(_DWORD *)(a1 + 4 * v2 + 710584);
  if ( v2 - 1 < 0 )
  {
    PalDebugPrintf(0, 0, "not stack");
    v2 = *(_DWORD *)(a1 + 738512);
  }
  *(_DWORD *)(a1 + 738512) = v2 - 1;
  v4 = *(_DWORD *)(a1 + 4 * (v2 - 1) + 710584);
  if ( v3 == -1 )
  {
    v5 = 0;
  }
  else
  {
    *(int *)((char *)&dword_57D45C + a1) = v3;
    v5 = 1;
    *(int *)((char *)&dword_57D460 + a1) = v4;
  }
  byte_57D459[a1] = v5;
  PalDebugPrintf(10, 0, "tips_set_color [%d] [%x][%x]", v5, v3, v4);
  return 1;
}
