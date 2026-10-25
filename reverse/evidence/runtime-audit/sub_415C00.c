int __cdecl sub_415C00(int a1)
{
  int v1; // ecx
  int v2; // ecx
  int v3; // ebx
  int v4; // ecx
  int v5; // ecx
  int v6; // ecx
  int v7; // ecx
  int v8; // ecx
  HWND WindowHandle; // eax
  int result; // eax
  char *v11; // ebx
  int v12; // eax
  int v13; // esi
  int v14; // [esp+Ch] [ebp-120h]
  int v15; // [esp+10h] [ebp-11Ch]
  int v16; // [esp+14h] [ebp-118h]
  int v17; // [esp+18h] [ebp-114h]
  int v18; // [esp+1Ch] [ebp-110h]
  int v19; // [esp+20h] [ebp-10Ch]
  CHAR v20[260]; // [esp+24h] [ebp-108h] BYREF

  memset(v20, 0, sizeof(v20));
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
  v14 = *(_DWORD *)(a1 + 4 * v4 + 710584);
  if ( v4 - 1 < 0 )
  {
    PalDebugPrintf(0, 0, "not stack");
    v4 = *(_DWORD *)(a1 + 738512);
  }
  v5 = v4 - 1;
  *(_DWORD *)(a1 + 738512) = v5;
  v15 = *(_DWORD *)(a1 + 4 * v5 + 710584);
  if ( v5 - 1 < 0 )
  {
    PalDebugPrintf(0, 0, "not stack");
    v5 = *(_DWORD *)(a1 + 738512);
  }
  v6 = v5 - 1;
  *(_DWORD *)(a1 + 738512) = v6;
  v16 = *(_DWORD *)(a1 + 4 * v6 + 710584);
  if ( v6 - 1 < 0 )
  {
    PalDebugPrintf(0, 0, "not stack");
    v6 = *(_DWORD *)(a1 + 738512);
  }
  v7 = v6 - 1;
  *(_DWORD *)(a1 + 738512) = v7;
  v17 = *(_DWORD *)(a1 + 4 * v7 + 710584);
  if ( v7 - 1 < 0 )
  {
    PalDebugPrintf(0, 0, "not stack");
    v7 = *(_DWORD *)(a1 + 738512);
  }
  v8 = v7 - 1;
  *(_DWORD *)(a1 + 738512) = v8;
  v18 = *(_DWORD *)(a1 + 4 * v8 + 710584);
  if ( v8 - 1 < 0 )
  {
    PalDebugPrintf(0, 0, "not stack");
    v8 = *(_DWORD *)(a1 + 738512);
  }
  *(_DWORD *)(a1 + 738512) = v8 - 1;
  v19 = *(_DWORD *)(a1 + 4 * (v8 - 1) + 710584);
  sub_44B120(v20, v3, 0x104u);
  if ( lstrlenA(&v20[4]) <= 32 )
  {
    v11 = (char *)&dword_54CE40 + a1;
    v12 = PalMemoryAlloc(68, 0);
    *(int *)((char *)&dword_54CE40 + a1) = v12;
    v13 = v12;
    *(_DWORD *)(v12 + 24) = PalCheckWindowMode();
    *(_DWORD *)(v13 + 28) = v15;
    *(_DWORD *)(v13 + 32) = v16;
    *(_DWORD *)(v13 + 36) = v17;
    *(_DWORD *)(v13 + 40) = v18;
    *(_DWORD *)(v13 + 16) = 1920;
    *(_DWORD *)(v13 + 20) = 1080;
    *(_DWORD *)(v13 + 44) = v19;
    if ( !sub_415330((int *)((char *)&dword_54CE40 + a1)) )
    {
      *(_DWORD *)(v13 + 48) = *(_DWORD *)(v13 + 28);
      *(_DWORD *)(v13 + 52) = *(_DWORD *)(v13 + 32);
      *(_DWORD *)(v13 + 56) = *(_DWORD *)(v13 + 36);
      *(_DWORD *)(v13 + 60) = *(_DWORD *)(v13 + 40);
      *(_DWORD *)(v13 + 64) = *(_DWORD *)(v13 + 44);
    }
    *((_DWORD *)v11 + 1) = 2 * v14;
    strncpy(v11 + 16, &v20[4], 2 * v14);
    PalAttachProcess(sub_415670, v11);
    PalDebugPrintf(10, 0, "player_name_set_begin %s, %d, %d, %d, %d, %d, %d", &v20[4], v14, v15, v16, v17, v18, v19);
    SetWindowTextA(*(HWND *)v13, &v20[4]);
    ShowWindow(*(HWND *)v13, 5);
    UpdateWindow(*(HWND *)v13);
    result = 1;
    *(_DWORD *)(v13 + 12) = 1;
  }
  else
  {
    WindowHandle = (HWND)PalGetWindowHandle();
    MessageBoxA(WindowHandle, &byte_49FFA0, "player_name_set_begin", 0x30u);
    return 0;
  }
  return result;
}
