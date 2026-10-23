int __cdecl sub_4275F0(int a1)
{
  int v2; // ecx
  int v3; // ecx
  int v4; // edx
  unsigned int v5; // edi
  int v6; // ebx
  int v7; // ecx
  int *v8; // esi
  int *v9; // eax
  int v10; // eax
  int v11; // esi
  _DWORD *v12; // eax
  int v13; // ecx
  int v14; // eax
  int v16; // [esp+Ch] [ebp-8h]
  int v17; // [esp+Ch] [ebp-8h]
  int v18; // [esp+10h] [ebp-4h]
  int v19; // [esp+1Ch] [ebp+8h]

  v2 = *(_DWORD *)(a1 + 738512);
  if ( v2 - 1 < 0 )
  {
    PalDebugPrintf(0, 0, "not stack");
    v2 = *(_DWORD *)(a1 + 738512);
  }
  v3 = v2 - 1;
  *(_DWORD *)(a1 + 738512) = v3;
  v4 = *(_DWORD *)(a1 + 4 * v3 + 710584);
  v19 = v4;
  if ( v3 - 1 < 0 )
  {
    PalDebugPrintf(0, 0, "not stack");
    v3 = *(_DWORD *)(a1 + 738512);
    v4 = v19;
  }
  *(_DWORD *)(a1 + 738512) = v3 - 1;
  v5 = *(_DWORD *)(a1 + 4 * (v3 - 1) + 710584);
  if ( (unsigned int)(v4 + 1) <= 0x81 )
  {
    if ( v5 >= 0x10 )
      v5 = 0;
    v6 = dword_49C320[v5];
    if ( v4 == -1 )
    {
      v7 = 128;
      v8 = (int *)(a1 + 666400);
      v18 = 128;
      do
      {
        v9 = (int *)*(v8 - 3);
        if ( v9 )
        {
          v16 = *v9;
          if ( (sub_44AC00(v7) & 0x30) == 0 )
            PalSpriteSetOption(v16, v6);
          v10 = *v8 | (1 << v5);
          if ( !v6 )
            v10 = 0;
          v7 = v18;
          *v8 = v10;
        }
        v8 += 86;
        v18 = --v7;
      }
      while ( v7 );
    }
    else
    {
      v11 = 344 * v4 + a1;
      v12 = *(_DWORD **)(v11 + 666388);
      if ( !v12 )
      {
LABEL_25:
        PalDebugPrintf(10, 0, "set_filter  %d, %d", v4, v5);
        return 1;
      }
      PalDebugPrintf(0, 0, "[%d][%d]AdvSpriteSetOption", *v12, dword_49C320[v5]);
      v17 = **(_DWORD **)(v11 + 666388);
      if ( (sub_44AC00(v13) & 0x30) == 0 )
        PalSpriteSetOption(v17, v6);
      v14 = *(_DWORD *)(v11 + 666400) | (1 << v5);
      if ( !v6 )
        v14 = 0;
      *(_DWORD *)(v11 + 666400) = v14;
    }
    v4 = v19;
    goto LABEL_25;
  }
  PalDebugPrintf(0, 0, aGraphError, v4);
  return 0;
}