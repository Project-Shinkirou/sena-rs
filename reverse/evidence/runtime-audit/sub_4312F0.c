int __cdecl sub_4312F0(int a1)
{
  int v1; // edx

  v1 = *(_DWORD *)(PalTaskGetTaskData(0) + 24);
  *(_DWORD *)(a1 + 4 * *(_DWORD *)(a1 + 738516) + 710840) = v1;
  PalDebugPrintf(10, 0, "[%d] get_new_savefile", v1);
  return 1;
}
