int __cdecl sub_1027BD60(int *a1, int a2, int a3, int a4, int a5, int a6, float a7, float a8)
{
  int *v8; // edi
  int v9; // ecx
  unsigned int v10; // ebx
  unsigned __int16 v11; // dx
  unsigned int v12; // eax
  __m64 v13; // mm6
  __m64 v14; // mm7
  int v15; // ecx
  unsigned int v16; // ebx
  unsigned __int16 v17; // dx
  unsigned int v18; // eax
  __m64 v19; // mm4
  __m64 v20; // mm5
  unsigned int *v21; // esi
  int result; // eax
  int j; // [esp+D4h] [ebp-138h]
  int i; // [esp+E0h] [ebp-12Ch]
  int v25; // [esp+F8h] [ebp-114h]
  unsigned int v26; // [esp+104h] [ebp-108h]
  int v27; // [esp+104h] [ebp-108h]
  int *v28; // [esp+140h] [ebp-CCh]
  int v29; // [esp+140h] [ebp-CCh]
  int *v30; // [esp+14Ch] [ebp-C0h]
  int v31; // [esp+14Ch] [ebp-C0h]
  unsigned int v32; // [esp+158h] [ebp-B4h]
  int v33; // [esp+158h] [ebp-B4h]
  int v34; // [esp+164h] [ebp-A8h]
  int v35; // [esp+170h] [ebp-9Ch]
  int v36; // [esp+170h] [ebp-9Ch]
  int v37; // [esp+17Ch] [ebp-90h]
  int v38; // [esp+188h] [ebp-84h]
  int v39; // [esp+194h] [ebp-78h]
  int v40; // [esp+1A0h] [ebp-6Ch]
  int v41; // [esp+1ACh] [ebp-60h]
  int v42; // [esp+1B8h] [ebp-54h]
  int v43; // [esp+1C4h] [ebp-48h]
  int v44; // [esp+1C4h] [ebp-48h]
  int v45; // [esp+1E8h] [ebp-24h]
  int v46; // [esp+1F4h] [ebp-18h]
  int *v47; // [esp+200h] [ebp-Ch]

  v46 = (int)(float)((float)a5 * a7);
  if ( v46 > a2 )
    v46 = a2;
  v45 = (int)(float)((float)a6 * a8);
  if ( v45 > a3 )
    v45 = a3;
  sub_1011E892(COERCE_INT(128.0 / a7));
  v42 = sub_101196DA();
  sub_1011E892(COERCE_INT(128.0 / a8));
  v41 = sub_101196DA();
  v35 = a2 - v46;
  v25 = 0;
  if ( dword_150AE96C )
  {
    v36 = 4 * v35;
    v8 = a1;
    v9 = v45 << 16;
    do
    {
      v26 = 0;
      v10 = 128 - (v25 & 0x7F);
      LOBYTE(v11) = v25 & 0x7F;
      HIBYTE(v11) = v25 & 0x7F;
      v12 = v11 << 16;
      LOWORD(v12) = v11;
      v13 = _m_punpcklbw(_mm_cvtsi32_si64(v12), 0LL);
      LOBYTE(v11) = 0x80 - (v25 & 0x7F);
      HIBYTE(v11) = v11;
      LOWORD(v10) = v11;
      v10 <<= 16;
      LOWORD(v10) = v11;
      v14 = _m_punpcklbw(_mm_cvtsi32_si64(v10), 0LL);
      v43 = a5 * ((unsigned int)v25 >> 7);
      v15 = v46 | v9;
      do
      {
        v16 = 128 - (v26 & 0x7F);
        LOBYTE(v17) = v26 & 0x7F;
        HIBYTE(v17) = v26 & 0x7F;
        v18 = v17 << 16;
        LOWORD(v18) = v17;
        v19 = _m_punpcklbw(_mm_cvtsi32_si64(v18), 0LL);
        LOBYTE(v17) = 0x80 - (v26 & 0x7F);
        HIBYTE(v17) = v17;
        LOWORD(v16) = v17;
        v16 <<= 16;
        LOWORD(v16) = v17;
        v20 = _m_punpcklbw(_mm_cvtsi32_si64(v16), 0LL);
        v21 = (unsigned int *)(4 * (v43 + (v26 >> 7)) + a4);
        v32 = *v21;
        v30 = (int *)*v21;
        v28 = (int *)*v21;
        if ( a5 - 1 != v26 >> 7 )
          v32 = v21[1];
        if ( a6 - 1 < (int)((unsigned int)v25 >> 7) )
        {
          v30 = &a5 + (_DWORD)v21;
          if ( a5 - 1 != v26 >> 7 )
            v28 = &a6 + (_DWORD)v21;
        }
        *v8++ = _mm_cvtsi64_si32(
                  _m_packuswb(
                    _m_paddusw(
                      _m_psrlwi(
                        _m_pmullw(
                          _m_psrlwi(
                            _m_paddusw(
                              _m_pmullw(_m_punpcklbw(_mm_cvtsi32_si64(*v21), 0LL), v20),
                              _m_pmullw(_m_punpcklbw(_mm_cvtsi32_si64(v32), 0LL), v19)),
                            7u),
                          v14),
                        7u),
                      _m_psrlwi(
                        _m_pmullw(
                          _m_psrlwi(
                            _m_paddusw(
                              _m_pmullw(_m_punpcklbw(_mm_cvtsi32_si64((unsigned int)v30), 0LL), v20),
                              _m_pmullw(_m_punpcklbw(_mm_cvtsi32_si64((unsigned int)v28), 0LL), v19)),
                            7u),
                          v13),
                        7u)),
                    0LL));
        v26 += v42;
        LOWORD(v15) = v15 - 1;
      }
      while ( (_WORD)v15 );
      v8 = (int *)((char *)v8 + v36);
      result = v41;
      v25 += v41;
      v9 = v15 - 0x10000;
    }
    while ( v9 );
    _m_empty();
  }
  else
  {
    for ( i = 0; ; ++i )
    {
      result = i;
      if ( i >= v45 )
        break;
      v27 = 0;
      v38 = v25 & 0x7F;
      v44 = a5 * (v25 >> 7);
      for ( j = 0; j < v46; ++j )
      {
        v40 = v27 & 0x7F;
        v47 = (int *)(a4 + 4 * (v27 >> 7) + 4 * v44);
        v29 = *v47;
        v31 = *v47;
        v33 = *v47;
        if ( v27 >> 7 != a5 - 1 )
          v33 = v47[1];
        if ( v25 >> 7 != a6 - 1 )
        {
          v31 = v47[a5];
          if ( v27 >> 7 != a5 - 1 )
            v29 = v47[a5 + 1];
        }
        v37 = 128 - v38;
        v39 = 128 - v40;
        v34 = *v47;
        *a1++ = (unsigned __int8)((v38 * (v40 * (unsigned __int8)v29 + v39 * (unsigned __int8)v31)
                                 + v37 * (v40 * (unsigned __int8)v33 + v39 * (unsigned __int8)v34)) >> 14) | ((unsigned __int8)((v38 * (v40 * BYTE1(v29) + v39 * BYTE1(v31)) + v37 * (v40 * BYTE1(v33) + v39 * BYTE1(v34))) >> 14) << 8) | ((unsigned __int8)((v38 * (v40 * BYTE2(v29) + v39 * BYTE2(v31)) + v37 * (v40 * BYTE2(v33) + v39 * BYTE2(v34))) >> 14) << 16) | ((unsigned __int8)((v38 * (v40 * HIBYTE(v29) + v39 * HIBYTE(v31)) + v37 * (v40 * HIBYTE(v33) + v39 * HIBYTE(v34))) >> 14) << 24);
        v27 += v42;
      }
      a1 += v35;
      v25 += v41;
    }
  }
  return result;
}
