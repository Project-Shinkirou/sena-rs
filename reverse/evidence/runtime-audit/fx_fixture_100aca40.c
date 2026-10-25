// IDA 9.0 pseudocode from testcase/dll/Pal.dll, address 0x100aca40.
// Build identity: reverse/evidence/native-interface-inventory.json.
int __cdecl sub_100ACA40(_DWORD *a1, int a2, int a3)
{
  unsigned int v3; // ecx
  unsigned int v4; // esi
  int v5; // edi
  float v6; // xmm3_4
  unsigned int v7; // edx
  float v8; // xmm4_4
  float v9; // xmm1_4
  float v10; // xmm7_4
  float v11; // xmm4_4
  int v13; // ecx
  int v15; // [esp+28h] [ebp-C0h]
  int v16; // [esp+2Ch] [ebp-BCh]
  float v17; // [esp+40h] [ebp-A8h]
  _DWORD v18[16]; // [esp+48h] [ebp-A0h] BYREF
  float v19[5]; // [esp+88h] [ebp-60h] BYREF
  float v20; // [esp+9Ch] [ebp-4Ch]
  float v21; // [esp+A0h] [ebp-48h]
  float v22; // [esp+A4h] [ebp-44h]
  int v23; // [esp+A8h] [ebp-40h]
  int v24; // [esp+ACh] [ebp-3Ch]
  float v25; // [esp+B0h] [ebp-38h]
  float v26; // [esp+B4h] [ebp-34h]
  float v27; // [esp+B8h] [ebp-30h]
  float v28; // [esp+BCh] [ebp-2Ch]
  int v29; // [esp+C0h] [ebp-28h]
  int v30; // [esp+C4h] [ebp-24h]
  float v31; // [esp+C8h] [ebp-20h]
  float v32; // [esp+CCh] [ebp-1Ch]
  float v33; // [esp+D0h] [ebp-18h]
  float v34; // [esp+D4h] [ebp-14h]
  int v35; // [esp+D8h] [ebp-10h]
  int v36; // [esp+DCh] [ebp-Ch]
  float v37; // [esp+E0h] [ebp-8h]
  float v38; // [esp+E4h] [ebp-4h]

  v3 = a1[12];
  v4 = a1[192];
  v5 = a1[2];
  v19[2] = 0.0;
  v19[3] = NAN;
  v23 = 0;
  v24 = -1;
  v29 = 0;
  v30 = -1;
  v6 = (float)v3;
  v7 = a1[193];
  v8 = (double)((v3 - v4) >> 1) + 0.0;
  v9 = (float)(unsigned int)a1[13];
  v17 = (float)(v8 + 0.5) / v6;
  v10 = (float)v4;
  v20 = (float)((float)((double)((a1[13] - v7) >> 1) + 0.0) + 0.5) / v9;
  v21 = v10 / v6;
  v26 = v20;
  v25 = (float)((float)(v10 - 0.5) / v6) + v17;
  v11 = (float)v7;
  v19[4] = v17;
  v19[1] = v11 / v9;
  v22 = v11 / v9;
  v19[0] = -(float)(v10 / v6);
  v27 = v19[0];
  v28 = -(float)(v11 / v9);
  v31 = v17;
  v32 = (float)((float)(v11 - 0.5) / v9) + v20;
  v33 = v10 / v6;
  v34 = v28;
  v35 = 0;
  v36 = -1;
  v37 = v25;
  v38 = v32;
  memset(&v18[11], 0, 16);
  memset(&v18[6], 0, 16);
  memset(&v18[1], 0, 16);
  v18[15] = 1065353216;
  v18[10] = 1065353216;
  v18[5] = 1065353216;
  v18[0] = 1065353216;
  (*(void (__stdcall **)(int, int, _DWORD *))(*(_DWORD *)v5 + 176))(v5, 256, v18);
  (*(void (__stdcall **)(int, int, _DWORD *))(*(_DWORD *)v5 + 176))(v5, 2, v18);
  (*(void (__stdcall **)(int, int, _DWORD *))(*(_DWORD *)v5 + 176))(v5, 3, v18);
  (*(void (__stdcall **)(_DWORD, _DWORD, _DWORD))(*(_DWORD *)a1[2] + 148))(a1[2], 0, a1[8]);
  (*(void (__stdcall **)(_DWORD, _DWORD, _DWORD, int, _DWORD, int, _DWORD))(*(_DWORD *)a1[2] + 172))(
    a1[2],
    0,
    0,
    1,
    0,
    1065353216,
    0);
  if ( !sub_100AC2C0(v13, a1[4], a1[12], a1[13], a2, a3) )
    sub_100AC230(v15, v16, a2, a3);
  (*(void (__stdcall **)(int, _DWORD, _DWORD))(*(_DWORD *)v5 + 148))(v5, 0, a1[5]);
  (*(void (__stdcall **)(int, _DWORD, _DWORD))(*(_DWORD *)v5 + 260))(v5, 0, a1[6]);
  (*(void (__stdcall **)(int, int, int, float *, int))(*(_DWORD *)v5 + 332))(v5, 5, 2, v19, 24);
  return 1;
}
