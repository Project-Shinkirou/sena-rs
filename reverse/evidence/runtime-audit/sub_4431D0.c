int __cdecl sub_4431D0(int a1)
{
  int v2; // ecx
  int v3; // ecx
  int v4; // ecx
  int v5; // edi
  int v6; // ecx
  int v7; // eax
  int v8; // ebx
  int v9; // edx
  int *v10; // esi
  int v11; // ebx
  int v12; // eax
  int v14; // [esp+Ch] [ebp-4h]
  int v15; // [esp+18h] [ebp+8h]

  v2 = *(_DWORD *)(a1 + 738512);
  if ( v2 - 1 < 0 )
  {
    PalDebugPrintf(0, 0, "not stack");
    v2 = *(_DWORD *)(a1 + 738512);
  }
  v3 = v2 - 1;
  *(_DWORD *)(a1 + 738512) = v3;
  v15 = *(_DWORD *)(a1 + 4 * v3 + 710584);
  if ( v3 - 1 < 0 )
  {
    PalDebugPrintf(0, 0, "not stack");
    v3 = *(_DWORD *)(a1 + 738512);
  }
  v4 = v3 - 1;
  *(_DWORD *)(a1 + 738512) = v4;
  v5 = *(_DWORD *)(a1 + 4 * v4 + 710584);
  if ( v4 - 1 < 0 )
  {
    PalDebugPrintf(0, 0, "not stack");
    v4 = *(_DWORD *)(a1 + 738512);
  }
  *(_DWORD *)(a1 + 738512) = v4 - 1;
  v6 = *(_DWORD *)(a1 + 4 * (v4 - 1) + 710584);
  v7 = v15;
  v14 = v6;
  if ( v15 == -1 )
  {
    v8 = 8;
    v9 = 0;
  }
  else
  {
    v8 = v15 + 1;
    v9 = v15;
    if ( __OFSUB__(v15, v15 + 1) )
      goto LABEL_16;
  }
  v10 = (int *)(124 * v9 + a1 + 658056);
  v11 = v8 - v9;
  do
  {
    v12 = *(v10 - 21);
    if ( v12 && *v10 != v5 )
    {
      *v10 = v5;
      PalSoundPause(v12, v5, v6);
      v6 = v14;
    }
    v10 += 31;
    --v11;
  }
  while ( v11 );
  v7 = v15;
LABEL_16:
  PalDebugPrintf(10, 0, "bgv_pause %d, %d, %d", v7, v5, v6);
  return 1;
}