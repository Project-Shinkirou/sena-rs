int __cdecl sub_441E50(int a1)
{
  void (*v1)(_DWORD, _DWORD, const char *, ...); // edx
  int v2; // ecx
  int v3; // edi
  int v4; // eax
  int i; // esi
  int v6; // ecx
  int v7; // eax
  _BYTE v9[4]; // [esp+8h] [ebp-1004h] BYREF
  char v10[4092]; // [esp+Ch] [ebp-1000h] BYREF

  v1 = (void (*)(_DWORD, _DWORD, const char *, ...))PalDebugPrintf;
  v2 = *(_DWORD *)(a1 + 738512);
  if ( v2 - 1 < 0 )
  {
    PalDebugPrintf(0, 0, "not stack");
    v2 = *(_DWORD *)(a1 + 738512);
    v1 = (void (*)(_DWORD, _DWORD, const char *, ...))PalDebugPrintf;
  }
  v3 = -1;
  *(_DWORD *)(a1 + 738512) = v2 - 1;
  v4 = *(_DWORD *)(a1 + 4 * (v2 - 1) + 710584);
  if ( (v4 & 0x10000000) != 0 )
  {
    sub_44B120(v9, v4, 0x1000u);
    for ( i = 0; i < *(int *)((char *)dword_54CEC8 + a1); v3 = v7 )
    {
      v6 = strcmp(v10, (const char *)&unk_54CF05 + 6188 * i + a1);
      if ( v6 )
        v6 = v6 < 0 ? -1 : 1;
      v7 = i;
      if ( v6 )
        v7 = v3;
      ++i;
    }
    PalDebugPrintf(10, 0, "tips_search [%s] %d", v10, v3);
  }
  else
  {
    v1(0, 0, aTipsSearchStri);
  }
  *(_DWORD *)(a1 + 4 * *(_DWORD *)(a1 + 738516) + 710840) = v3;
  return 1;
}