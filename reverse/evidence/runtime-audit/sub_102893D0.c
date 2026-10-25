int __cdecl sub_102893D0(int *a1, int a2)
{
  int v3; // [esp+D8h] [ebp-38h]
  void *Block; // [esp+E4h] [ebp-2Ch]
  int v5; // [esp+F0h] [ebp-20h]
  int v6; // [esp+FCh] [ebp-14h]
  int v7; // [esp+108h] [ebp-8h]

  v5 = sub_1011C4E3();
  v7 = (int)(float)((float)*a1 * 0.5);
  v6 = (int)(float)((float)a1[1] * 0.5);
  v3 = 4 * v6 * v7;
  Block = j__malloc(8 * v6 * v7);
  if ( !Block )
    return 0;
  j__memset(Block, 0, 8 * v6 * v7);
  sub_1011D6E0((int)Block + v3, v7, v6, v5, *a1, a1[1], 1056964608, 1056964608);
  sub_10120269((int)Block, (int)Block + v3, v7, v6, v7, v6, a2);
  sub_10118505(v5, *a1, a1[1], (int)Block, v7, v6, 0x40000000, 0x40000000);
  j__free(Block);
  return 1;
}
