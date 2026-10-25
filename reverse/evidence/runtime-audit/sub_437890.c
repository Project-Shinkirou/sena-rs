int __cdecl sub_437890(int a1)
{
  PalInputClear();
  PalInputKeyRepeatClear();
  *(_DWORD *)(a1 + 4192) = 0;
  PalDebugPrintf(10, 0, "input_clear");
  return 1;
}