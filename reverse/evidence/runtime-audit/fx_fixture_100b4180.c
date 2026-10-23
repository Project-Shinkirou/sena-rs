// IDA 9.0 pseudocode from testcase/dll/Pal.dll, address 0x100b4180.
// Build identity: reverse/evidence/native-interface-inventory.json.
int __cdecl sub_100B4180(int a1, float a2, float *a3)
{
  int result; // eax
  float v4; // xmm0_4
  float v5; // xmm1_4
  unsigned int v6; // [esp+18h] [ebp-4h]
  float v7; // [esp+28h] [ebp+Ch]

  a3[1] = *(float *)(a1 + 4) * a2;
  a3[2] = *(float *)(a1 + 8) * a2;
  if ( (dword_101294DC & 1) == 0 )
  {
    dword_101294DC |= 1u;
    dword_101294D8 = PaltimeGetTime();
  }
  v6 = PaltimeGetTime() - dword_101294D8;
  if ( v6 < *(_DWORD *)a1 )
  {
    v4 = (float)v6;
    result = *(_DWORD *)a1 >> 31;
    v5 = (float)*(unsigned int *)a1;
    v7 = v4 / v5;
  }
  else
  {
    result = PaltimeGetTime();
    dword_101294D8 = result;
    v7 = 1.0;
  }
  *a3 = v7;
  return result;
}
