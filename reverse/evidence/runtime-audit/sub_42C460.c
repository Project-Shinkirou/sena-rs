int __cdecl sub_42C460(int a1)
{
  int v2; // ecx
  int v3; // ecx
  int v4; // ecx
  int v5; // ecx
  int v6; // esi
  int v7; // ecx
  char *v8; // esi
  char *v9; // ebx
  char *v10; // ecx
  char v11; // al
  int v12; // ecx
  char v13; // al
  int v14; // ecx
  char v15; // al
  unsigned int v17; // [esp+Ch] [ebp-Ch]
  unsigned int v18; // [esp+10h] [ebp-8h]
  int v19; // [esp+14h] [ebp-4h]
  int v20; // [esp+14h] [ebp-4h]
  int v21; // [esp+20h] [ebp+8h]

  v2 = *(_DWORD *)(a1 + 738512);
  if ( v2 - 1 < 0 )
  {
    PalDebugPrintf(0, 0, "not stack");
    v2 = *(_DWORD *)(a1 + 738512);
  }
  v3 = v2 - 1;
  *(_DWORD *)(a1 + 738512) = v3;
  v19 = *(_DWORD *)(a1 + 4 * v3 + 710584);
  if ( v3 - 1 < 0 )
  {
    PalDebugPrintf(0, 0, "not stack");
    v3 = *(_DWORD *)(a1 + 738512);
  }
  v4 = v3 - 1;
  *(_DWORD *)(a1 + 738512) = v4;
  v21 = *(_DWORD *)(a1 + 4 * v4 + 710584);
  if ( v4 - 1 < 0 )
  {
    PalDebugPrintf(0, 0, "not stack");
    v4 = *(_DWORD *)(a1 + 738512);
  }
  v5 = v4 - 1;
  *(_DWORD *)(a1 + 738512) = v5;
  v6 = *(_DWORD *)(a1 + 4 * v5 + 710584);
  if ( v5 - 1 < 0 )
  {
    PalDebugPrintf(0, 0, "not stack");
    v5 = *(_DWORD *)(a1 + 738512);
  }
  *(_DWORD *)(a1 + 738512) = v5 - 1;
  v7 = *(_DWORD *)(a1 + 4 * (v5 - 1) + 710584);
  if ( (v21 & v6 & v7 & 0x10000000) != 0 )
  {
    v17 = v7 & 0xEFFFFFFF;
    v18 = v6 & 0xEFFFFFFF;
    v20 = a1 + 4428 * v19;
    v8 = (char *)(v20 + 88707);
    v9 = (char *)(v20 + 92803);
    PalDebugPrintf(
      10,
      0,
      "history_get_text %s %s %s",
      (const char *)(v20 + 88452),
      (const char *)(v20 + 92803),
      (const char *)(v20 + 88707));
    v10 = (char *)(v20 + 88452);
    do
    {
      v11 = *v10++;
      v10[a1 + 771295 + 2047 * (v21 & 0xEFFFFFFF) - (v20 + 88452)] = v11;
    }
    while ( v11 );
    v12 = a1 + 2047 * v18 - (_DWORD)v9;
    do
    {
      v13 = *v9++;
      v9[v12 + 771295] = v13;
    }
    while ( v13 );
    v14 = a1 + 2047 * v17 - (_DWORD)v8;
    do
    {
      v15 = *v8++;
      v8[v14 + 771295] = v15;
    }
    while ( v15 );
    *(_DWORD *)(a1 + 4 * *(_DWORD *)(a1 + 738516) + 710840) = *(_DWORD *)(v20 + 92868);
    return 1;
  }
  else
  {
    PalDebugPrintf(0, 0, aHistoryGetText_0);
    *(_DWORD *)(a1 + 4 * *(_DWORD *)(a1 + 738516) + 710840) = 0;
    return 0;
  }
}