int __cdecl sub_441DC0(int a1)
{
  char *v1; // esi

  v1 = (char *)dword_54CEC8 + a1;
  memset((char *)&dword_54CEC8[1547 * *(int *)((char *)dword_54CEC8 + a1) + 4] + a1, 0, 0x182Cu);
  *(_DWORD *)v1 = 0;
  *((_WORD *)v1 + 6) = 1;
  memset((char *)&dword_54CEC8[4] + a1, 0, 0x182Cu);
  PalDebugPrintf(10, 0, "tips_reset %d", *(int *)((char *)dword_54CEC8 + a1));
  return 1;
}