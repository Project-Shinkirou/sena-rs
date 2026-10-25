int __cdecl sub_10279DA0(_BYTE *a1, _DWORD *a2, int a3, int a4, int a5, int a6, int a7)
{
  unsigned __int8 *v8; // [esp+E8h] [ebp-F0h]
  unsigned __int8 *v9; // [esp+E8h] [ebp-F0h]
  unsigned __int8 *v10; // [esp+E8h] [ebp-F0h]
  unsigned __int8 *v11; // [esp+E8h] [ebp-F0h]
  _DWORD *v13; // [esp+F4h] [ebp-E4h]
  _BYTE *v14; // [esp+100h] [ebp-D8h]
  int v16; // [esp+118h] [ebp-C0h]
  unsigned int v17; // [esp+124h] [ebp-B4h]
  unsigned int v18; // [esp+124h] [ebp-B4h]
  unsigned int v19; // [esp+130h] [ebp-A8h]
  unsigned int v20; // [esp+130h] [ebp-A8h]
  unsigned int v21; // [esp+13Ch] [ebp-9Ch]
  unsigned int v22; // [esp+13Ch] [ebp-9Ch]
  unsigned int v23; // [esp+148h] [ebp-90h]
  unsigned int v24; // [esp+148h] [ebp-90h]
  int i; // [esp+154h] [ebp-84h]
  int n; // [esp+154h] [ebp-84h]
  int m; // [esp+154h] [ebp-84h]
  int k; // [esp+154h] [ebp-84h]
  int mm; // [esp+154h] [ebp-84h]
  int kk; // [esp+154h] [ebp-84h]
  int jj; // [esp+154h] [ebp-84h]
  int v32; // [esp+160h] [ebp-78h]
  int v33; // [esp+160h] [ebp-78h]
  int j; // [esp+16Ch] [ebp-6Ch]
  int ii; // [esp+16Ch] [ebp-6Ch]
  unsigned int v36; // [esp+178h] [ebp-60h]
  unsigned int v37; // [esp+178h] [ebp-60h]
  int v38; // [esp+184h] [ebp-54h]
  int v39; // [esp+184h] [ebp-54h]
  int v40; // [esp+184h] [ebp-54h]
  int v41; // [esp+184h] [ebp-54h]
  int v42; // [esp+184h] [ebp-54h]
  int v43; // [esp+184h] [ebp-54h]
  _DWORD v44[18]; // [esp+190h] [ebp-48h]
  char v45; // [esp+1F8h] [ebp+20h]

  for ( i = 0; i < 16; ++i )
    v44[i] = (unsigned __int8)byte_103CB518[16 * a7 + i];
  v45 = a7 + 1;
  v16 = 4 * (a5 - a3);
  v14 = &unk_103E1CA0;
  v32 = 0;
LABEL_5:
  if ( v32 < a4 )
  {
    for ( j = 0; ; ++j )
    {
      if ( j >= a3 )
      {
        v14 += v16;
        a2 = (_DWORD *)((char *)a2 + v16);
        ++v32;
        goto LABEL_5;
      }
      v23 = 0;
      v17 = 0;
      v19 = 0;
      v21 = 0;
      v8 = (unsigned __int8 *)(a2 - 8);
      v36 = 0;
      if ( j - 8 >= 0 )
      {
        if ( j + 8 < a3 )
        {
          for ( k = 0; k < 16; ++k )
          {
            v40 = v44[k];
            v21 += v40 * *v8;
            v19 += v40 * v8[1];
            v17 += v40 * v8[2];
            v23 += v40 * v8[3];
            v8 += 4;
          }
          *v14 = v21 >> v45;
          v14[1] = v19 >> v45;
          v14[2] = v17 >> v45;
          v14[3] = v23 >> v45;
        }
        else
        {
          for ( m = 0; m < a3 - j + 8; ++m )
          {
            v39 = v44[m];
            if ( v8[3] )
            {
              v21 += v39 * *v8;
              v19 += v39 * v8[1];
              v17 += v39 * v8[2];
              v23 += v39 * v8[3];
            }
            v36 += v39;
            v8 += 4;
          }
          if ( v23 )
          {
LABEL_15:
            *v14 = v21 / v36;
            v14[1] = v19 / v36;
            v14[2] = v17 / v36;
            v14[3] = v23 / v36;
            goto LABEL_29;
          }
          *(_DWORD *)v14 = *a2;
        }
      }
      else
      {
        v9 = &v8[4 * (8 - j)];
        for ( n = 8 - j; n < 16; ++n )
        {
          v38 = v44[n];
          if ( v9[3] )
          {
            v21 += v38 * *v9;
            v19 += v38 * v9[1];
            v17 += v38 * v9[2];
            v23 += v38 * v9[3];
          }
          v36 += v38;
          v9 += 4;
        }
        if ( v23 )
          goto LABEL_15;
        *(_DWORD *)v14 = *a2;
      }
LABEL_29:
      v14 += 4;
      ++a2;
    }
  }
  v13 = &unk_103E1CA0;
  v33 = 0;
LABEL_32:
  if ( v33 < a4 )
  {
    for ( ii = 0; ; ++ii )
    {
      if ( ii >= a3 )
      {
        a1 += v16;
        v13 = (_DWORD *)((char *)v13 + v16);
        ++v33;
        goto LABEL_32;
      }
      v24 = 0;
      v18 = 0;
      v20 = 0;
      v22 = 0;
      v10 = (unsigned __int8 *)&v13[-8 * a5];
      v37 = 0;
      if ( v33 - 8 >= 0 )
      {
        if ( v33 + 8 < a4 )
        {
          for ( jj = 0; jj < 16; ++jj )
          {
            v43 = v44[jj];
            v22 += v43 * *v10;
            v20 += v43 * v10[1];
            v18 += v43 * v10[2];
            v24 += v43 * v10[3];
            v10 += 4 * a5;
          }
          *a1 = v22 >> v45;
          a1[1] = v20 >> v45;
          a1[2] = v18 >> v45;
          a1[3] = v24 >> v45;
        }
        else
        {
          for ( kk = 0; kk < a4 - v33 + 8; ++kk )
          {
            v42 = v44[kk];
            if ( v10[3] )
            {
              v22 += v42 * *v10;
              v20 += v42 * v10[1];
              v18 += v42 * v10[2];
              v24 += v42 * v10[3];
            }
            v37 += v42;
            v10 += 4 * a5;
          }
          if ( v24 )
          {
LABEL_42:
            *a1 = v22 / v37;
            a1[1] = v20 / v37;
            a1[2] = v18 / v37;
            a1[3] = v24 / v37;
            goto LABEL_56;
          }
          *(_DWORD *)a1 = *v13;
        }
      }
      else
      {
        v11 = &v10[(8 - v33) * 4 * a5];
        for ( mm = 8 - v33; mm < 16; ++mm )
        {
          v41 = v44[mm];
          if ( v11[3] )
          {
            v22 += v41 * *v11;
            v20 += v41 * v11[1];
            v18 += v41 * v11[2];
            v24 += v41 * v11[3];
          }
          v37 += v41;
          v11 += 4 * a5;
        }
        if ( v24 )
          goto LABEL_42;
        *(_DWORD *)a1 = *v13;
      }
LABEL_56:
      a1 += 4;
      ++v13;
    }
  }
  return 1;
}
