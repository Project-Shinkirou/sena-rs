int __cdecl sub_443100(int a1)
{
  int v2; // [esp-4h] [ebp-8h]

  v2 = 100 * *(_DWORD *)(a1 + 659616) / 10000;
  *(_DWORD *)(a1 + 4 * *(_DWORD *)(a1 + 738516) + 710840) = v2;
  PalDebugPrintf(10, 0, "[%d] get_bgv_volume", v2);
  return 1;
}