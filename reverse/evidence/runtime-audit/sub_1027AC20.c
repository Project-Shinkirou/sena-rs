int __usercall sub_1027AC20@<eax>(__m64 a1@<mm5>, int *a2, int a3, int a4, int a5)
{
  unsigned int *v5; // edi
  __m64 v6; // mm2
  __m64 v7; // mm3
  int v8; // ecx
  unsigned int v9; // eax
  unsigned int v10; // ebx
  int v11; // ecx
  int j; // [esp+D0h] [ebp-60h]
  int i; // [esp+DCh] [ebp-54h]
  int v15; // [esp+E8h] [ebp-48h]
  unsigned __int16 v16; // [esp+F4h] [ebp-3Ch]

  v15 = a3 * a4;
  if ( a5 )
  {
    if ( dword_150AE96C )
    {
      v5 = (unsigned int *)a2;
      v6 = _m_punpcklbw(_mm_cvtsi32_si64(0x30601u), 0LL);
      v7 = _mm_cvtsi32_si64(0x10001u);
      v8 = a3 * a4;
      do
      {
        a1 = _m_pmaddwd(_m_psrlqi(_m_packssdw(a1, _m_pmaddwd(_m_punpcklbw(_mm_cvtsi32_si64(*v5), 0LL), v6)), 0x20u), v7);
        v9 = 4 * (_mm_cvtsi64_si32(a1) / 0xAu);
        v10 = HIWORD(*v5);
        LOBYTE(v10) = *(_BYTE *)(a5 + v9);
        v10 <<= 16;
        BYTE1(v10) = *(_BYTE *)(a5 + v9 + 1);
        LOBYTE(v10) = *(_BYTE *)(a5 + v9 + 2);
        *v5++ = v10;
        --v8;
      }
      while ( v8 );
      _m_empty();
    }
    else
    {
      for ( i = 0; i < v15; ++i )
      {
        v16 = (unsigned __int16)((unsigned __int8)*a2 + 6 * (unsigned __int8)BYTE1(*a2)
                                                      + 3 * (unsigned __int8)BYTE2(*a2))
            / 10;
        *a2 = *(unsigned __int8 *)(a5 + 4 * v16 + 2) | (*(unsigned __int8 *)(a5 + 4 * v16 + 1) << 8) | (*(unsigned __int8 *)(a5 + 4 * v16) << 16) | (HIBYTE(*a2) << 24);
        ++a2;
      }
    }
  }
  else
  {
    for ( j = 0; j < v15; ++j )
    {
      v11 = (unsigned __int8)((unsigned __int16)((unsigned __int8)*a2
                                               + 6 * (unsigned __int8)BYTE1(*a2)
                                               + 3 * (unsigned __int8)BYTE2(*a2))
                            / 10);
      *a2 = v11 | (v11 << 8) | (v11 << 16) | (HIBYTE(*a2) << 24);
      ++a2;
    }
  }
  return 1;
}
