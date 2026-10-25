int __cdecl sub_43DDB0(_DWORD *a1)
{
  int v2; // ecx
  int v3; // ecx
  int v4; // ecx
  int v5; // ecx
  int v6; // esi
  int v8; // [esp+Ch] [ebp-8h]
  int v9; // [esp+10h] [ebp-4h]
  int v10; // [esp+1Ch] [ebp+8h]

  v2 = a1[184628];
  if ( v2 - 1 < 0 )
  {
    PalDebugPrintf(0, 0, "not stack");
    v2 = a1[184628];
  }
  v3 = v2 - 1;
  a1[184628] = v3;
  v10 = a1[v3 + 177646];
  if ( v3 - 1 < 0 )
  {
    PalDebugPrintf(0, 0, "not stack");
    v3 = a1[184628];
  }
  v4 = v3 - 1;
  a1[184628] = v4;
  v9 = a1[v4 + 177646];
  if ( v4 - 1 < 0 )
  {
    PalDebugPrintf(0, 0, "not stack");
    v4 = a1[184628];
  }
  v5 = v4 - 1;
  a1[184628] = v5;
  v8 = a1[v5 + 177646];
  if ( v5 - 1 < 0 )
  {
    PalDebugPrintf(0, 0, "not stack");
    v5 = a1[184628];
  }
  a1[184628] = v5 - 1;
  v6 = a1[v5 + 177645];
  memset(a1 + 17, 0, 0x1014u);
  a1[13] = v10;
  a1[14] = v9;
  a1[15] = v8;
  a1[16] = v6;
  PalDebugPrintf(10, 0, "text_init_visualnovelmode %d, %d, %d, %d", v10, v9, v8, v6);
  return 1;
}
