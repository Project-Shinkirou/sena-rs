int __cdecl sub_441400(int a1)
{
  int v2; // ecx
  int v3; // ecx
  int v4; // ecx
  int v5; // ecx
  int v6; // ecx
  int v7; // ecx
  int v8; // ecx
  int v9; // ecx
  bool v10; // zf
  unsigned int v11; // ecx
  unsigned int v12; // ecx
  int Active; // eax
  void *v14; // ecx
  float *v15; // eax
  int v16; // esi
  char v17; // dl
  char v18; // cl
  int v19; // edx
  void *v20; // ecx
  int v21; // esi
  float *v23; // [esp+Ch] [ebp-4h]
  int v24; // [esp+18h] [ebp+8h]

  v2 = *(_DWORD *)(a1 + 738512);
  if ( v2 - 1 < 0 )
  {
    PalDebugPrintf(0, 0, "not stack");
    v2 = *(_DWORD *)(a1 + 738512);
  }
  v3 = v2 - 1;
  *(_DWORD *)(a1 + 738512) = v3;
  *(_DWORD *)(a1 + 16) = *(_DWORD *)(a1 + 4 * v3 + 710584);
  if ( v3 - 1 < 0 )
  {
    PalDebugPrintf(0, 0, "not stack");
    v3 = *(_DWORD *)(a1 + 738512);
  }
  *(_DWORD *)(a1 + 738512) = v3 - 1;
  *(_DWORD *)(a1 + 24) = *(_DWORD *)(a1 + 4 * (v3 - 1) + 710584);
  v4 = *(_DWORD *)(a1 + 738512);
  if ( v4 - 1 < 0 )
  {
    PalDebugPrintf(0, 0, "not stack");
    v4 = *(_DWORD *)(a1 + 738512);
  }
  *(_DWORD *)(a1 + 738512) = v4 - 1;
  *(_DWORD *)(a1 + 28) = *(_DWORD *)(a1 + 4 * (v4 - 1) + 710584);
  v5 = *(_DWORD *)(a1 + 738512);
  if ( v5 - 1 < 0 )
  {
    PalDebugPrintf(0, 0, "not stack");
    v5 = *(_DWORD *)(a1 + 738512);
  }
  *(_DWORD *)(a1 + 738512) = v5 - 1;
  *(_DWORD *)(a1 + 32) = *(_DWORD *)(a1 + 4 * (v5 - 1) + 710584);
  v6 = *(_DWORD *)(a1 + 738512);
  if ( v6 - 1 < 0 )
  {
    PalDebugPrintf(0, 0, "not stack");
    v6 = *(_DWORD *)(a1 + 738512);
  }
  *(_DWORD *)(a1 + 738512) = v6 - 1;
  *(_DWORD *)(a1 + 36) = *(_DWORD *)(a1 + 4 * (v6 - 1) + 710584);
  v7 = *(_DWORD *)(a1 + 738512);
  if ( v7 - 1 < 0 )
  {
    PalDebugPrintf(0, 0, "not stack");
    v7 = *(_DWORD *)(a1 + 738512);
  }
  *(_DWORD *)(a1 + 738512) = v7 - 1;
  *(_DWORD *)(a1 + 40) = *(_DWORD *)(a1 + 4 * (v7 - 1) + 710584);
  v8 = *(_DWORD *)(a1 + 738512);
  if ( v8 - 1 < 0 )
  {
    PalDebugPrintf(0, 0, "not stack");
    v8 = *(_DWORD *)(a1 + 738512);
  }
  *(_DWORD *)(a1 + 738512) = v8 - 1;
  *(_DWORD *)(a1 + 44) = *(_DWORD *)(a1 + 4 * (v8 - 1) + 710584);
  v9 = *(_DWORD *)(a1 + 738512);
  if ( v9 - 1 < 0 )
  {
    PalDebugPrintf(0, 0, "not stack");
    v9 = *(_DWORD *)(a1 + 738512);
  }
  *(_DWORD *)(a1 + 738512) = v9 - 1;
  *(_DWORD *)(a1 + 20) = *(_DWORD *)(a1 + 4 * (v9 - 1) + 710584);
  *(_WORD *)(a1 + 48) = -1;
  v11 = *(_DWORD *)(a1 + 32) & 0x80000003;
  v10 = v11 == 0;
  if ( *(int *)(a1 + 32) < 0 )
  {
    v12 = ((_BYTE)v11 - 1) | 0xFFFFFFFC;
    v10 = v12 == -1;
    v11 = v12 + 1;
  }
  if ( !v10 )
    *(_DWORD *)(a1 + 32) = *(_DWORD *)(a1 + 32) - v11 + 4;
  *(_DWORD *)(a1 + 4244) = *(_DWORD *)(a1 + 24);
  *(_DWORD *)(a1 + 4248) = *(_DWORD *)(a1 + 28);
  *(_DWORD *)(a1 + 4184) = 0;
  *(_DWORD *)(a1 + 4) = 1;
  PalFontSetFontSize(*(unsigned __int16 *)(a1 + 16));
  sub_43A1B0(
    *(_DWORD *)(a1 + 16) + *(_DWORD *)(a1 + 32),
    *(_DWORD *)(a1 + 36),
    *(_DWORD *)(a1 + 16),
    *(_DWORD *)(a1 + 20),
    (_DWORD *)(a1 + 4296));
  Active = PalRenderGetActive();
  v14 = *(void **)(a1 + 4260);
  v24 = Active;
  if ( v14 )
    sub_449190(v14);
  v15 = (float *)sub_4492A0(0, 4);
  v16 = *(_DWORD *)(a1 + 36);
  v17 = 0;
  v23 = v15;
  if ( ((v16 - 1) & v16) != 0 )
  {
    for ( ; v16; v16 >>= 1 )
      ++v17;
    v16 = 2 << (v17 - 1);
  }
  v18 = 0;
  v19 = *(_DWORD *)(a1 + 32) + *(_DWORD *)(a1 + 16);
  if ( ((v19 - 1) & v19) != 0 )
  {
    for ( ; v19; v19 >>= 1 )
      ++v18;
    v19 = 2 << (v18 - 1);
  }
  *(_DWORD *)v15 = PalSpriteCreate(v24, v19, v16, 4867);
  v23[4] = *(float *)(a1 + 24);
  v23[5] = *(float *)(a1 + 28);
  v23[27] = (float)*(int *)(a1 + 24);
  v23[28] = (float)*(int *)(a1 + 28);
  *(_DWORD *)(a1 + 4260) = v23;
  v20 = *(void **)(a1 + 4256);
  if ( v20 )
    sub_449190(v20);
  v21 = sub_4492A0(0, 5);
  *(_DWORD *)v21 = PalSpriteCreate(v24, 1024, 64, 4867);
  *(_DWORD *)(v21 + 16) = *(_DWORD *)(a1 + 40);
  *(_DWORD *)(v21 + 20) = *(_DWORD *)(a1 + 44);
  *(float *)(v21 + 108) = (float)*(int *)(a1 + 40);
  *(float *)(v21 + 112) = (float)*(int *)(a1 + 44);
  *(_DWORD *)(a1 + 4256) = v21;
  PalDebugPrintf(
    10,
    0,
    "text_init %d, %d, %d, %d, %d, %d, %d, %d",
    *(_DWORD *)(a1 + 16),
    *(_DWORD *)(a1 + 24),
    *(_DWORD *)(a1 + 28),
    *(_DWORD *)(a1 + 32),
    *(_DWORD *)(a1 + 36),
    *(_DWORD *)(a1 + 40),
    *(_DWORD *)(a1 + 44),
    *(_DWORD *)(a1 + 20));
  return 1;
}
