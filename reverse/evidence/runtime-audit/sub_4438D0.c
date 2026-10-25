int __cdecl sub_4438D0(int a1)
{
  int v1; // ecx
  int v2; // ecx
  int v3; // ecx
  int v4; // ecx
  int v5; // ebx
  int v6; // edx
  int v7; // eax
  int v8; // ebx
  int v9; // eax
  CHAR *v11; // ecx
  int v12; // ebx
  CHAR v13; // al
  int v14; // [esp+Ch] [ebp-58h]
  int v15; // [esp+14h] [ebp-50h]
  int v16; // [esp+18h] [ebp-4Ch]
  int v17; // [esp+1Ch] [ebp-48h]
  CHAR String[64]; // [esp+20h] [ebp-44h] BYREF

  v1 = *(_DWORD *)(a1 + 738512);
  if ( v1 - 1 < 0 )
  {
    PalDebugPrintf(0, 0, "not stack");
    v1 = *(_DWORD *)(a1 + 738512);
  }
  v2 = v1 - 1;
  *(_DWORD *)(a1 + 738512) = v2;
  v17 = *(_DWORD *)(a1 + 4 * v2 + 710584);
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
  v14 = *(_DWORD *)(a1 + 4 * v4 + 710584);
  if ( v4 - 1 < 0 )
  {
    PalDebugPrintf(0, 0, "not stack");
    v4 = *(_DWORD *)(a1 + 738512);
  }
  *(_DWORD *)(a1 + 738512) = v4 - 1;
  v5 = *(_DWORD *)(a1 + 4 * (v4 - 1) + 710584);
  v16 = v5;
  memset(String, 0, sizeof(String));
  v6 = v15;
  if ( v15 != -1 || v5 )
  {
    v8 = 40 * v17 + a1;
    *(_DWORD *)(v8 + 658968) = v16;
    if ( v16 )
      v9 = v15;
    else
      v9 = sub_449120(v15);
    *(_DWORD *)(a1 + 40 * v17 + 658964) = v9;
    sub_44B1E0(String, v14, 0);
    if ( lstrlenA(String) > 32 )
    {
      PalDebugPrintf(0, 0, "set_voice_autopan : size over[%s]", String);
      return 0;
    }
    v11 = String;
    v12 = v8 - (_DWORD)String;
    do
    {
      v13 = *v11++;
      v11[v12 + 658971] = v13;
    }
    while ( v13 );
    v5 = v16;
    v6 = v15;
  }
  else
  {
    v7 = 5 * v17;
    *(_OWORD *)(a1 + 8 * v7 + 658964) = 0LL;
    *(_OWORD *)(a1 + 8 * v7 + 658980) = 0LL;
    *(_QWORD *)(a1 + 8 * v7 + 658996) = 0LL;
  }
  PalDebugPrintf(10, 0, "set_voice_autopan  %d, %d, %s, %d", v17, v6, String, v5);
  return 1;
}
