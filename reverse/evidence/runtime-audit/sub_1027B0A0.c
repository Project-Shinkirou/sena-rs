int __cdecl sub_1027B0A0(int a1, int *a2, int a3, int a4)
{
  _QWORD *v4; // edi
  int result; // eax
  int v6; // edx
  int i; // ecx
  int k; // [esp+D0h] [ebp-64h]
  int j; // [esp+DCh] [ebp-58h]
  int v10; // [esp+E8h] [ebp-4Ch]
  __int64 v11; // [esp+F4h] [ebp-40h]
  int v12; // [esp+104h] [ebp-30h]
  int v13; // [esp+110h] [ebp-24h]
  int v14; // [esp+110h] [ebp-24h]
  int v15; // [esp+11Ch] [ebp-18h]
  int v16; // [esp+128h] [ebp-Ch]
  _DWORD *v17; // [esp+13Ch] [ebp+8h]
  int v18; // [esp+148h] [ebp+14h]
  int v19; // [esp+148h] [ebp+14h]

  v16 = *a2;
  v15 = a2[1];
  v13 = a2[2] - *a2;
  v12 = a2[3] - v15;
  v17 = (_DWORD *)(a1 + 4 * (*a2 + v15 * a4));
  v18 = a4 - v13;
  if ( dword_150AE96C )
  {
    LODWORD(v11) = a3;
    HIDWORD(v11) = a3;
    v19 = 4 * v18;
    v10 = v13 & 1;
    v14 = v13 >> 1;
    v4 = v17;
    result = a3;
    v6 = a2[3] - a2[1];
    do
    {
      for ( i = v14; i; --i )
        *v4++ = v11;
      if ( v10 )
      {
        *(_DWORD *)v4 = a3;
        v4 = (_QWORD *)((char *)v4 + 4);
      }
      v4 = (_QWORD *)((char *)v4 + v19);
      --v6;
    }
    while ( v6 );
    _m_empty();
  }
  else
  {
    for ( j = v15; ; ++j )
    {
      result = j;
      if ( j >= v12 )
        break;
      for ( k = v16; k < v13; ++k )
        *v17++ = a3;
      v17 += v18;
    }
  }
  return result;
}
