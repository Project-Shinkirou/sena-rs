// IDA 9.0 pseudocode from testcase/dll/Pal.dll, address 0x100a1680.
// Build identity: reverse/evidence/native-interface-inventory.json.
int __fastcall sub_100A1680(
        int *a1,
        unsigned int a2,
        unsigned int a3,
        int a4,
        unsigned int a5,
        unsigned int a6,
        float a7,
        float a8,
        float a9)
{
  bool v10; // cf
  int v12; // edx
  int v13; // edi
  __m128i v14; // xmm0
  float v15; // xmm0_4
  float v16; // xmm0_4
  int result; // eax
  int v18; // ebx
  int v19; // ebx
  int v20; // esi
  int v21; // edx
  int v22; // ebx
  int v23; // esi
  int v24; // eax
  int v25; // ebx
  int v26; // edx
  int v27; // eax
  int v28; // ebx
  int v29; // esi
  int v30; // edx
  int v31; // eax
  int v32; // ebx
  int v33; // [esp+Ch] [ebp-14h]
  int v34; // [esp+10h] [ebp-10h]
  int v35; // [esp+14h] [ebp-Ch]
  int v36; // [esp+18h] [ebp-8h]
  int v37; // [esp+1Ch] [ebp-4h]
  int v38; // [esp+28h] [ebp+8h]
  int v39; // [esp+28h] [ebp+8h]
  int v40; // [esp+28h] [ebp+8h]
  int v41; // [esp+30h] [ebp+10h]
  int v42; // [esp+34h] [ebp+14h]
  int v43; // [esp+38h] [ebp+18h]
  __int16 v44; // [esp+3Ch] [ebp+1Ch]
  __int16 v45; // [esp+40h] [ebp+20h]

  v10 = a2 < a5;
  v12 = a6;
  v13 = a5;
  if ( v10 )
    v13 = a2;
  v14 = _mm_cvtsi32_si128(a6);
  if ( a3 < a6 )
    v12 = a3;
  v37 = 4 * (a2 - v13);
  v42 = v12;
  v36 = 4 * (a5 - v13);
  v15 = _mm_cvtepi32_pd(v14).m128d_f64[0] + qword_10113690[a6 >> 31];
  v35 = (int)(float)((float)(v15 * a8) * 0.1);
  v16 = (float)a5;
  result = (int)(float)((float)(v16 * a9) * 0.1);
  v34 = result;
  v18 = (int)(float)(a7 * 511.0);
  v44 = v18;
  if ( v35 )
  {
    if ( result )
    {
      v19 = 0;
      v38 = 0;
      if ( v12 > 0 )
      {
        v20 = a4;
        do
        {
          result = (v35 * dword_101123D0[((_WORD)v19 + v44) & 0x1FF]) >> 16;
          v33 = result;
          if ( v13 > 0 )
          {
            v21 = (v35 * dword_101123D0[((_WORD)v19 + v44) & 0x1FF]) >> 16;
            v45 = v44 - result;
            v19 = v38;
            v43 = v13;
            do
            {
              result = (v34 * dword_101123D0[((_WORD)v21 + v45) & 0x1FF]) >> 16;
              v22 = result + v19;
              if ( v22 >= 0 && v22 < v42 && v21 >= 0 && v21 < v13 )
              {
                result = *(_DWORD *)(v20 + 4 * (v33 + a5 * result));
                *a1 = result;
              }
              v19 = v38;
              ++a1;
              v20 += 4;
              ++v21;
              --v43;
            }
            while ( v43 );
            v12 = v42;
          }
          a1 = (int *)((char *)a1 + v37);
          ++v19;
          v20 += v36;
          v38 = v19;
        }
        while ( v19 < v12 );
      }
    }
    else
    {
      result = 0;
      v39 = 0;
      if ( v12 > 0 )
      {
        v23 = a4;
        do
        {
          v24 = ((_WORD)v18 + (_WORD)result) & 0x1FF;
          v25 = (v35 * dword_101123D0[v24]) >> 16;
          if ( v13 > 0 )
          {
            v26 = (v35 * dword_101123D0[v24]) >> 16;
            v41 = v13;
            v27 = v13;
            do
            {
              if ( v26 >= 0 && v26 < v13 )
              {
                *a1 = *(_DWORD *)(v23 + 4 * v25);
                v27 = v41;
              }
              ++a1;
              v23 += 4;
              ++v26;
              v41 = --v27;
            }
            while ( v27 );
            v12 = v42;
          }
          a1 = (int *)((char *)a1 + v37);
          result = v39 + 1;
          v23 += v36;
          LOWORD(v18) = v44;
          v39 = result;
        }
        while ( result < v12 );
      }
    }
  }
  else
  {
    v28 = 0;
    v40 = 0;
    if ( v12 > 0 )
    {
      v29 = a4;
      result = v12;
      do
      {
        v30 = 0;
        if ( v13 > 0 )
        {
          do
          {
            v31 = (v34 * dword_101123D0[((_WORD)v30 + v44) & 0x1FF]) >> 16;
            v32 = v31 + v28;
            if ( v32 >= 0 && v32 < v42 )
              *a1 = *(_DWORD *)(v29 + 4 * a5 * v31);
            v28 = v40;
            ++v30;
            ++a1;
            v29 += 4;
          }
          while ( v30 < v13 );
          result = v42;
        }
        a1 = (int *)((char *)a1 + v37);
        ++v28;
        v29 += v36;
        v40 = v28;
      }
      while ( v28 < result );
    }
  }
  return result;
}
