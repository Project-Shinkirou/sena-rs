// IDA 9.0 pseudocode from testcase/dll/Pal.dll, address 0x100ac2c0.
// Build identity: reverse/evidence/native-interface-inventory.json.
int __fastcall sub_100AC2C0(int a1, int a2, int a3, int a4, unsigned int a5, unsigned int a6, int a7, _DWORD *a8)
{
  float v10; // xmm0_4

  if ( !a7 )
  {
    if ( (*(_BYTE *)dword_1012C8E0 & 1) == 0 )
      return 0;
    sub_100AC1A0(a1, a4);
    return 1;
  }
  if ( a7 != 1 )
  {
    if ( a7 == 2 )
    {
      if ( (*(_BYTE *)dword_1012C8E0 & 4) != 0 )
      {
        v10 = (float)a6;
        ((void (__cdecl *)(_DWORD, _DWORD, _DWORD, _DWORD, _DWORD, _DWORD, _DWORD))sub_100ABF60)(
          a1,
          a4,
          (float)a5,
          LODWORD(v10),
          *a8,
          a8[1],
          a8[2]);
        return 1;
      }
      return 0;
    }
    return 1;
  }
  if ( (*(_BYTE *)dword_1012C8E0 & 2) != 0 )
  {
    sub_100AC0D0(a1, a4, *a8, a8[1], a8[2]);
  }
  else
  {
    (*(void (__stdcall **)(_DWORD, _DWORD, _DWORD))(**(_DWORD **)(a1 + 8) + 148))(
      *(_DWORD *)(a1 + 8),
      0,
      *(_DWORD *)(a1 + 32));
    (*(void (__stdcall **)(_DWORD, _DWORD, _DWORD, int, _DWORD, int, _DWORD))(**(_DWORD **)(a1 + 8) + 172))(
      *(_DWORD *)(a1 + 8),
      0,
      0,
      1,
      0,
      1065353216,
      0);
    (*(void (__stdcall **)(_DWORD, _DWORD, int))(**(_DWORD **)(a1 + 8) + 260))(*(_DWORD *)(a1 + 8), 0, a4);
    (*(void (__stdcall **)(_DWORD, int))(**(_DWORD **)(a1 + 8) + 356))(*(_DWORD *)(a1 + 8), 322);
    (*(void (__stdcall **)(_DWORD, int, int, int, int))(**(_DWORD **)(a1 + 8) + 332))(*(_DWORD *)(a1 + 8), 5, 2, a2, 24);
    (*(void (__stdcall **)(_DWORD, _DWORD, _DWORD))(**(_DWORD **)(a1 + 8) + 148))(
      *(_DWORD *)(a1 + 8),
      0,
      *(_DWORD *)(a1 + 20));
    sub_100AC050(a8[2]);
  }
  return 1;
}
