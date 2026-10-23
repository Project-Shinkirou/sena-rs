int __cdecl sub_1029AA00(int *a1, int a2)
{
  int v3; // [esp+D8h] [ebp-50h]
  void *Block; // [esp+E4h] [ebp-44h]
  int v5; // [esp+F0h] [ebp-38h]
  int v6; // [esp+FCh] [ebp-2Ch]
  int v7; // [esp+108h] [ebp-20h]
  int v8; // [esp+114h] [ebp-14h]
  int v9; // [esp+120h] [ebp-8h]

  v5 = sub_10119FCC(a1[2]);
  if ( !v5 )
    return 0;
  v9 = *a1;
  v8 = a1[1];
  v7 = *(_DWORD *)(a1[2] + 4);
  v6 = *(_DWORD *)(a1[2] + 8);
  v3 = 4 * v6 * v7;
  Block = j__malloc(8 * v6 * v7);
  if ( !Block )
    return 0;
  j__memset(Block, 0, 8 * v6 * v7);
  sub_1011D6E0((int)Block + v3, v7, v6, v5, *(_DWORD *)(a1[2] + 4), *(_DWORD *)(a1[2] + 8), 1065353216, 1065353216);
  sub_10120269((int)Block, (int)Block + v3, v9, v8, v7, v6, a2);
  sub_10118505(v5, *(_DWORD *)(a1[2] + 4), *(_DWORD *)(a1[2] + 8), (int)Block, v7, v6, 1065353216, 1065353216);
  j__free(Block);
  sub_1011DB13(a1[2]);
  return 1;
}
