signed int __fastcall sub_442430(int a1, int a2, signed int *a3)
{
  signed int result; // eax
  int v6; // ecx
  __m128i v7; // xmm1
  float v8; // xmm1_4
  float v9; // xmm0_4
  int v10; // edx
  _BYTE v11[8]; // [esp+Ch] [ebp-158h] BYREF
  float v12; // [esp+14h] [ebp-150h]
  int v13; // [esp+30h] [ebp-134h]

  result = *a3;
  if ( a3[1] )
  {
    v6 = *(_DWORD *)(a1 + 6224);
    if ( result < 0 )
      goto LABEL_5;
    if ( result > 20 )
      goto LABEL_18;
    result *= 96;
    if ( result < 0 )
    {
LABEL_5:
      v6 = -v6;
      goto LABEL_18;
    }
    if ( result <= 1920 )
    {
      v7 = _mm_cvtsi32_si128(result);
      result = 2 * v6;
      v8 = _mm_cvtepi32_ps(v7).m128_f32[0] / 1920.0;
      v9 = (float)(2 * v6);
LABEL_17:
      v6 = (int)(float)((float)(v8 * v9) - (float)v6);
      goto LABEL_18;
    }
    goto LABEL_18;
  }
  if ( result == -1 )
    result = 0;
  else
    result = (signed int)&unk_4C0240 + 756 * result;
  if ( *(_DWORD *)(result + 12) )
  {
    result = *(_DWORD *)result;
    if ( result )
    {
      PalSpriteGetInfo(result, v11);
      result = (int)v12;
      v6 = *(_DWORD *)(a1 + 6224);
      v10 = (int)v12 + (v13 >> 1);
      if ( v10 < 0 )
      {
        v6 = -v6;
        goto LABEL_18;
      }
      if ( v10 <= 1920 )
      {
        result = 2 * v6;
        v9 = (float)v10 / 1920.0;
        v8 = (float)(2 * v6);
        goto LABEL_17;
      }
LABEL_18:
      if ( a3[2] != v6 )
      {
        a3[2] = v6;
        return PalSoundSetPanChannel(a2, v6);
      }
    }
  }
  return result;
}