int __cdecl sub_441E20(int a1)
{
  int v2; // [esp-4h] [ebp-4h]

  v2 = *(int *)((char *)dword_54CEC8 + a1);
  *(__int16 *)((char *)&word_54CED4 + a1) = 256;
  PalDebugPrintf(10, 0, "tips_stop %d", v2);
  return 1;
}