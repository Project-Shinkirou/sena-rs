int __cdecl sub_4420B0(int a1)
{
  int v2; // [esp-4h] [ebp-4h]

  v2 = (unsigned __int8)byte_57D458[a1];
  *(_DWORD *)(a1 + 4 * *(_DWORD *)(a1 + 738516) + 710840) = v2;
  byte_57D458[a1] ^= 1u;
  PalDebugPrintf(10, 0, "[%d]tips_pause", v2);
  return 1;
}