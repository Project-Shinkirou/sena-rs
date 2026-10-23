int __cdecl sub_10278DC0(_DWORD *a1, int a2, int a3, int a4, int a5)
{
  unsigned int *v5; // edi
  int v6; // esi
  int v7; // ebx
  __m64 v8; // mm1
  __m64 v9; // mm2
  int v10; // ecx
  int v11; // edx
  __m64 v12; // mm5
  __m64 v13; // mm3
  unsigned int *v14; // esi
  int i; // [esp+D0h] [ebp-78h]
  int m; // [esp+D0h] [ebp-78h]
  int ii; // [esp+D0h] [ebp-78h]
  int j; // [esp+DCh] [ebp-6Ch]
  int k; // [esp+DCh] [ebp-6Ch]
  int n; // [esp+DCh] [ebp-6Ch]
  int v22; // [esp+E8h] [ebp-60h]
  int v23; // [esp+F4h] [ebp-54h]
  int v24; // [esp+100h] [ebp-48h]
  int v25; // [esp+100h] [ebp-48h]
  int v26; // [esp+10Ch] [ebp-3Ch]
  int v27; // [esp+10Ch] [ebp-3Ch]
  int v28; // [esp+10Ch] [ebp-3Ch]
  int v29; // [esp+10Ch] [ebp-3Ch]
  unsigned int v30; // [esp+10Ch] [ebp-3Ch]
  int v31; // [esp+118h] [ebp-30h]
  int v32; // [esp+118h] [ebp-30h]
  int v33; // [esp+118h] [ebp-30h]
  int v34; // [esp+118h] [ebp-30h]
  unsigned int v35; // [esp+118h] [ebp-30h]
  int v36; // [esp+124h] [ebp-24h]
  int v37; // [esp+124h] [ebp-24h]
  int v38; // [esp+124h] [ebp-24h]
  int v39; // [esp+124h] [ebp-24h]
  unsigned int v40; // [esp+124h] [ebp-24h]
  unsigned __int8 v41; // [esp+130h] [ebp-18h]
  int *v42; // [esp+13Ch] [ebp-Ch]
  _DWORD *v43; // [esp+13Ch] [ebp-Ch]
  _DWORD *v44; // [esp+154h] [ebp+Ch]
  _DWORD *v45; // [esp+154h] [ebp+Ch]
  _DWORD *v46; // [esp+154h] [ebp+Ch]
  _DWORD *v47; // [esp+154h] [ebp+Ch]

  v44 = (_DWORD *)(a2 + 4 * a5 + 4);
  v42 = &a1[a5 + 1];
  v23 = a3 - 1;
  v22 = a4 - 1;
  v24 = a5 - a3 + 1;
  if ( dword_150AE96C )
  {
    v25 = 4 * v24;
    v5 = &a1[a5 + 1];
    v6 = (int)v44;
    v7 = a5;
    v8 = _mm_cvtsi32_si64(0xFF000000);
    v9 = _mm_cvtsi32_si64(0xFFFFFFu);
    v10 = a4 - 2;
    do
    {
      v11 = a3 - 2;
      do
      {
        v12 = _mm_cvtsi32_si64(*v5);
        v13 = _m_paddw(
                _m_paddw(
                  _m_paddw(
                    _m_psllwi(_m_punpcklbw(v12, 0LL), 2u),
                    _m_punpcklbw(_mm_cvtsi32_si64(*(_DWORD *)(v6 + 4)), 0LL)),
                  _m_punpcklbw(_mm_cvtsi32_si64(*(_DWORD *)(v6 - 4)), 0LL)),
                _m_punpcklbw(_mm_cvtsi32_si64(*(_DWORD *)(v6 + 4 * a5)), 0LL));
        v14 = (unsigned int *)(v6 - v7 * 4);
        *v5++ = _mm_cvtsi64_si32(
                  _m_por(
                    _m_pand(
                      _m_packuswb(_m_psrlwi(_m_paddw(v13, _m_punpcklbw(_mm_cvtsi32_si64(*v14), 0LL)), 3u), 0LL),
                      v9),
                    _m_pand(v12, v8)));
        v6 = (int)&v14[v7 + 1];
        --v11;
      }
      while ( v11 );
      v5 = (unsigned int *)((char *)v5 + v25);
      v6 += v25;
      --v10;
    }
    while ( v10 );
    _m_empty();
  }
  else
  {
    for ( i = 0; i < v22; ++i )
    {
      for ( j = 0; j < v23; ++j )
      {
        v41 = HIBYTE(*v44);
        v36 = 4 * (unsigned __int8)BYTE2(*v44);
        v31 = 4 * (unsigned __int8)BYTE1(*v44);
        v26 = 4 * (unsigned __int8)*v44;
        v45 = v44 + 1;
        v37 = v36 + (unsigned __int8)BYTE2(*v45);
        v32 = v31 + (unsigned __int8)BYTE1(*v45);
        v27 = v26 + (unsigned __int8)*v45;
        v45 -= 2;
        v38 = v37 + (unsigned __int8)BYTE2(*v45);
        v33 = v32 + (unsigned __int8)BYTE1(*v45);
        v28 = v27 + (unsigned __int8)*v45;
        v46 = &v45[a5 + 1];
        v39 = v38 + (unsigned __int8)BYTE2(*v46);
        v34 = v33 + (unsigned __int8)BYTE1(*v46);
        v29 = v28 + (unsigned __int8)*v46;
        v47 = &v46[-2 * a5];
        v40 = v39 + (unsigned __int8)BYTE2(*v47);
        v35 = v34 + (unsigned __int8)BYTE1(*v47);
        v30 = v29 + (unsigned __int8)*v47;
        v44 = &v47[a5 + 1];
        *v42++ = (unsigned __int8)(v30 >> 3) | ((unsigned __int8)(v35 >> 3) << 8) | ((unsigned __int8)(v40 >> 3) << 16) | (v41 << 24);
      }
      v44 += v24;
      v42 += v24;
    }
  }
  v43 = a1;
  for ( k = 0; k < v23; ++k )
  {
    *v43 = v43[a5];
    ++v43;
  }
  for ( m = 0; m < v22; ++m )
  {
    *v43 = *(v43 - 1);
    v43 += a5;
  }
  for ( n = 0; n < v23; ++n )
  {
    *v43 = v43[-a5];
    --v43;
  }
  for ( ii = 0; ii < v22; ++ii )
  {
    *v43 = v43[1];
    v43 -= a5;
  }
  return 1;
}
