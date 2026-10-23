int __cdecl sub_441A30(int a1)
{
  int v1; // eax
  int v2; // edx

  v1 = *(int *)((char *)&dword_54CEC8 + a1);
  v2 = *(_DWORD *)(a1 + 738516);
  if ( v1 )
    *(_DWORD *)(a1 + 4 * v2 + 710840) = v1 - 1;
  else
    *(_DWORD *)(a1 + 4 * v2 + 710840) = 0;
  return 1;
}