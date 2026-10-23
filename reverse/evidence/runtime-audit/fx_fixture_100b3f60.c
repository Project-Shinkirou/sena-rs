// IDA 9.0 pseudocode from testcase/dll/Pal.dll, address 0x100b3f60.
// Build identity: reverse/evidence/native-interface-inventory.json.
int __cdecl sub_100B3F60(int a1)
{
  int result; // eax

  *(_DWORD *)(a1 + 12) = 0;
  if ( *(_DWORD *)(a1 + 24) )
  {
    PalMemoryFree(*(void **)(a1 + 24));
    *(_DWORD *)(a1 + 24) = 0;
  }
  result = a1;
  if ( *(_DWORD *)(a1 + 20) )
  {
    PalMemoryFree(*(void **)(a1 + 20));
    result = a1;
    *(_DWORD *)(a1 + 20) = 0;
  }
  return result;
}
