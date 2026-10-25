int __cdecl sub_42BE30(int a1)
{
  int v1; // ecx
  int v3; // [esp-8h] [ebp-10h]

  v1 = *(_DWORD *)(a1 + 738512);
  if ( v1 - 1 < 0 )
  {
    PalDebugPrintf(0, 0, "not stack");
    v1 = *(_DWORD *)(a1 + 738512);
  }
  *(_DWORD *)(a1 + 738512) = v1 - 1;
  v3 = *(_DWORD *)(a1 + 4 * (v1 - 1) + 710584);
  *(_DWORD *)(a1 + 88372) = 1;
  sub_44B1E0((char *)(a1 + 88376), v3, 1);
  PalDebugPrintf(10, 0, "history_set %s", (const char *)(a1 + 88376));
  return 1;
}