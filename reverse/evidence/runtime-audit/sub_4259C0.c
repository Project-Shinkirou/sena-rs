int __cdecl sub_4259C0(int a1)
{
  int v1; // ecx
  int v2; // ecx
  int v3; // ebx
  int v4; // ecx
  int v5; // ecx
  int v6; // ecx
  int v7; // ecx
  int v8; // ecx
  int *v9; // eax
  int v10; // esi
  int v11; // eax
  int v12; // eax
  int **v14; // [esp+Ch] [ebp-120h]
  int v15; // [esp+10h] [ebp-11Ch]
  int v16; // [esp+14h] [ebp-118h] BYREF
  int v17; // [esp+18h] [ebp-114h]
  int v18; // [esp+1Ch] [ebp-110h]
  int v19; // [esp+20h] [ebp-10Ch]
  char Str1; // [esp+24h] [ebp-108h] BYREF
  CHAR String[259]; // [esp+25h] [ebp-107h] BYREF

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
  v4 = v2 - 1;
  *(_DWORD *)(a1 + 738512) = v4;
  v15 = *(_DWORD *)(a1 + 4 * v4 + 710584);
  if ( v4 - 1 < 0 )
  {
    PalDebugPrintf(0, 0, "not stack");
    v4 = *(_DWORD *)(a1 + 738512);
  }
  v5 = v4 - 1;
  *(_DWORD *)(a1 + 738512) = v5;
  v16 = *(_DWORD *)(a1 + 4 * v5 + 710584);
  if ( v5 - 1 < 0 )
  {
    PalDebugPrintf(0, 0, "not stack");
    v5 = *(_DWORD *)(a1 + 738512);
  }
  v6 = v5 - 1;
  *(_DWORD *)(a1 + 738512) = v6;
  v17 = *(_DWORD *)(a1 + 4 * v6 + 710584);
  if ( v6 - 1 < 0 )
  {
    PalDebugPrintf(0, 0, "not stack");
    v6 = *(_DWORD *)(a1 + 738512);
  }
  v7 = v6 - 1;
  *(_DWORD *)(a1 + 738512) = v7;
  v18 = *(_DWORD *)(a1 + 4 * v7 + 710584);
  if ( v7 - 1 < 0 )
  {
    PalDebugPrintf(0, 0, "not stack");
    v7 = *(_DWORD *)(a1 + 738512);
  }
  *(_DWORD *)(a1 + 738512) = v7 - 1;
  v8 = *(_DWORD *)(a1 + 4 * (v7 - 1) + 710584);
  v19 = v8;
  if ( (unsigned int)(v3 + 1) > 0x81 )
  {
    PalDebugPrintf(0, 0, aGraphError, v3);
    return 0;
  }
  else
  {
    v14 = (int **)(a1 + 344 * v3 + 666388);
    v9 = *v14;
    if ( *v14 )
    {
      if ( v15 == 0xFFFFFFF )
      {
        v10 = 0;
      }
      else
      {
        sub_44B1E0(&Str1, v15, 1);
        v11 = sub_44E370(String);
        v8 = v19;
        v10 = v11;
        v9 = *v14;
      }
      v12 = *v9;
      if ( v16 | v17 | v18 | v8 )
        PalSpritePaint(v12, &v16, v10);
      else
        PalSpritePaint(v12, 0, v10);
      PalDebugPrintf(10, 0, "sp_paint %d, 0x%08X, %d, %d, %d, %d", v3, v10, v16, v17, v18, v19);
    }
    return 1;
  }
}
