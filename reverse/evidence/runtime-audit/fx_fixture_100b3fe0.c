// IDA 9.0 pseudocode from testcase/dll/Pal.dll, address 0x100b3fe0.
// Build identity: reverse/evidence/native-interface-inventory.json.
_DWORD *__cdecl sub_100B3FE0(_DWORD *a1, _DWORD *a2)
{
  float v3; // xmm0_4
  float v4; // xmm1_4
  double v5; // [esp+8h] [ebp-28h]
  double v6; // [esp+10h] [ebp-20h]
  unsigned int v7; // [esp+24h] [ebp-Ch]
  float v8; // [esp+28h] [ebp-8h]

  if ( a2 && a1 )
  {
    if ( a1[3] )
    {
      v7 = PaltimeGetTime() - a1[1];
      if ( v7 < a1[2] )
      {
        v6 = (double)v7;
        v3 = (float)v7;
        v5 = (double)(unsigned int)a1[2];
        v4 = v5;
        v8 = v3 / v4;
      }
      else
      {
        v7 = a1[2];
        v8 = 1.0;
      }
      if ( *a1 )
        v8 = 1.0 - v8;
      if ( !a1[4] )
        sub_100B4180(a1[5], LODWORD(v8), a1[6]);
      if ( *a1 && v7 == a1[2] )
        a1[3] = 0;
      if ( Pal3DIs(LODWORD(v5), HIDWORD(v5), LODWORD(v6), HIDWORD(v6)) )
        *a2 = dword_1011A144[a1[4]];
      else
        *a2 = dword_1011A140[a1[4]];
      a2[1] = a1[6];
      if ( *((_DWORD *)dword_101294A4 + 191) )
        return a2;
      else
        return 0;
    }
    else
    {
      return 0;
    }
  }
  else
  {
    SetLastError(6u);
    return 0;
  }
}
