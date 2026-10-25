int __cdecl sub_4432E0(int a1)
{
  int v2; // ecx
  int v3; // ecx
  int v4; // edi
  int v5; // edx
  int v6; // ecx
  int *v7; // eax
  int v8; // ecx
  int v10; // [esp+14h] [ebp+8h]

  v2 = *(_DWORD *)(a1 + 738512);
  if ( v2 - 1 < 0 )
  {
    PalDebugPrintf(0, 0, "not stack");
    v2 = *(_DWORD *)(a1 + 738512);
  }
  v3 = v2 - 1;
  *(_DWORD *)(a1 + 738512) = v3;
  v4 = *(_DWORD *)(a1 + 4 * v3 + 710584);
  v10 = v4;
  if ( v3 - 1 < 0 )
  {
    PalDebugPrintf(0, 0, "not stack");
    v3 = *(_DWORD *)(a1 + 738512);
  }
  *(_DWORD *)(a1 + 738512) = v3 - 1;
  v5 = *(_DWORD *)(a1 + 4 * (v3 - 1) + 710584);
  if ( v4 == -1 )
  {
    v6 = 8;
    v4 = 0;
  }
  else
  {
    v6 = v4 + 1;
    if ( __OFSUB__(v4, v4 + 1) )
      goto LABEL_10;
  }
  v7 = (int *)(a1 + 124 * v4 + 658052);
  v8 = v6 - v4;
  do
  {
    *v7 = v5;
    v7 += 31;
    --v8;
  }
  while ( v8 );
LABEL_10:
  PalDebugPrintf(10, 0, "bgv_enable %d, %d", v10, v5);
  return 1;
}