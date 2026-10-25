int __cdecl sub_441A70(int a1)
{
  int v2; // ecx
  int v3; // ecx
  int v4; // ecx
  int v5; // ebx
  unsigned int v6; // ecx
  char *v7; // edx
  const char *v8; // ecx
  int v9; // edx
  char v10; // al
  const char *v12; // [esp+Ch] [ebp-4h]
  int v13; // [esp+18h] [ebp+8h]
  _DWORD *v14; // [esp+18h] [ebp+8h]

  v2 = *(_DWORD *)(a1 + 738512);
  if ( v2 - 1 < 0 )
  {
    PalDebugPrintf(0, 0, "not stack");
    v2 = *(_DWORD *)(a1 + 738512);
  }
  v3 = v2 - 1;
  *(_DWORD *)(a1 + 738512) = v3;
  v13 = *(_DWORD *)(a1 + 4 * v3 + 710584);
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
  v7 = (char *)&dword_54CED8[1547 * v13] + a1;
  v14 = v7;
  if ( (v5 & 0x10000000) == 0 )
  {
    PalDebugPrintf(0, 0, aTipsGetStrStri);
    goto LABEL_14;
  }
  if ( v6 > 2 )
  {
    PalDebugPrintf(0, 0, aTipsGetStrD, v6);
LABEL_14:
    *(_DWORD *)(a1 + 4 * *(_DWORD *)(a1 + 738516) + 710840) = 0;
    return 0;
  }
  v8 = &v7[2047 * v6 + 45];
  v12 = v8;
  v9 = 2047 * (v5 & 0xEFFFFFFF) - (_DWORD)v8 + a1;
  do
  {
    v10 = *v8++;
    v8[v9 + 771295] = v10;
  }
  while ( v10 );
  *(_DWORD *)(a1 + 4 * *(_DWORD *)(a1 + 738516) + 710840) = *v14;
  PalDebugPrintf(10, 0, "tips_get_str %s", v12);
  return 1;
}