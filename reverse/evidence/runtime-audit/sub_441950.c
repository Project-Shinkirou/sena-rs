int __cdecl sub_441950(_DWORD *a1)
{
  int v1; // ecx
  int v2; // ecx
  int v3; // ebx
  int v4; // edx
  int v5; // eax
  int v6; // eax

  v1 = a1[184628];
  if ( v1 - 1 < 0 )
  {
    PalDebugPrintf(0, 0, "not stack");
    v1 = a1[184628];
  }
  v2 = v1 - 1;
  a1[184628] = v2;
  v3 = a1[v2 + 177646];
  if ( v2 - 1 < 0 )
  {
    PalDebugPrintf(0, 0, "not stack");
    v2 = a1[184628];
  }
  a1[184628] = v2 - 1;
  v4 = a1[v2 + 177645];
  v5 = 0;
  if ( v3 )
    v5 = *(_DWORD *)(a1[177644] - 4 * v3 + a1[177640]);
  *(int *)((char *)&dword_54CECC + (_DWORD)a1) = v5;
  v6 = 0;
  if ( v4 )
    v6 = *(_DWORD *)(a1[177644] - 4 * v4 + a1[177640]);
  *(int *)((char *)&dword_54CED0 + (_DWORD)a1) = v6;
  memset((char *)dword_54CED8 + (_DWORD)a1, 0, 0x30580u);
  return 1;
}