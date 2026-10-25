int __cdecl sub_1027B7C0(_DWORD *a1, int a2, int a3, int a4, int a5, int a6, float a7, float a8)
{
  _DWORD *v8; // edi
  int v9; // ecx
  int v10; // ecx
  int j; // [esp+D4h] [ebp-84h]
  int i; // [esp+E0h] [ebp-78h]
  int v14; // [esp+F8h] [ebp-60h]
  int v15; // [esp+F8h] [ebp-60h]
  int v16; // [esp+104h] [ebp-54h]
  unsigned int v17; // [esp+110h] [ebp-48h]
  int v18; // [esp+110h] [ebp-48h]
  int v19; // [esp+11Ch] [ebp-3Ch]
  int v20; // [esp+128h] [ebp-30h]
  int v21; // [esp+134h] [ebp-24h]
  int v22; // [esp+140h] [ebp-18h]

  v22 = (int)(float)((float)a5 * a7);
  if ( v22 > a2 )
    v22 = a2;
  v21 = (int)(float)((float)a6 * a8);
  if ( v21 > a3 )
    v21 = a3;
  sub_1011E892(COERCE_INT(128.0 / a7));
  v20 = sub_101196DA();
  sub_1011E892(COERCE_INT(128.0 / a8));
  v19 = sub_101196DA();
  v14 = a2 - v22;
  v16 = 0;
  if ( dword_150AE96C )
  {
    v15 = 4 * v14;
    v8 = a1;
    v9 = v21 << 16;
    do
    {
      v17 = 0;
      v10 = v22 | v9;
      do
      {
        *v8++ = *(_DWORD *)(4 * a5 * ((unsigned int)v16 >> 7) + a4 + 4 * (v17 >> 7));
        v17 += v20;
        LOWORD(v10) = v10 - 1;
      }
      while ( (_WORD)v10 );
      v8 = (_DWORD *)((char *)v8 + v15);
      v16 += v19;
      v9 = v10 - 0x10000;
    }
    while ( v9 );
  }
  else
  {
    for ( i = 0; i < v21; ++i )
    {
      v18 = 0;
      for ( j = 0; j < v22; ++j )
      {
        *a1++ = *(_DWORD *)(a4 + 4 * a5 * (v16 >> 7) + 4 * (v18 >> 7));
        v18 += v20;
      }
      a1 += v14;
      v16 += v19;
    }
  }
  return 1;
}
