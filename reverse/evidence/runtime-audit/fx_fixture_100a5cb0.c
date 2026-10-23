// IDA 9.0 pseudocode from testcase/dll/Pal.dll, address 0x100a5cb0.
// Build identity: reverse/evidence/native-interface-inventory.json.
int __cdecl sub_100A5CB0(_DWORD *a1, int a2, _DWORD *a3)
{
  _DWORD *v3; // edi
  int v4; // edx
  _DWORD v6[9]; // [esp+14h] [ebp-24h] BYREF

  v3 = Block;
  if ( a2 == 1 )
    sub_100A1680(a1[10], *(_DWORD *)(*a1 + 16), a1[9], a1[10], *a3, a3[1], a3[2]);
  v4 = *a1;
  v6[7] = a1[9];
  v6[8] = a1[10];
  *(_DWORD *)(v4 + 84) = v3[1];
  v6[0] = 0;
  v6[1] = 0;
  v6[2] = 1065353216;
  v6[3] = 0;
  v6[4] = -1;
  v6[5] = 0;
  v6[6] = 0;
  sub_100A6AA0(v6);
  return 1;
}
