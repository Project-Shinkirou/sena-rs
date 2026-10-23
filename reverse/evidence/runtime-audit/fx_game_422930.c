// IDA database Game.sqlite, reference Game.exe, address 0x422930.
// Build identity: reverse/evidence/native-interface-inventory.json.
int __cdecl sub_422930(int a1)
{
  int v1; // ecx
  int v2; // ebx
  _DWORD *v3; // esi
  int v4; // eax
  char *v5; // eax
  int v6; // ecx
  _DWORD *v8; // [esp+10h] [ebp+8h]

  v1 = *(_DWORD *)(a1 + 738512);
  if ( v1 - 1 < 0 )
  {
    PalDebugPrintf(0, 0, "not stack");
    v1 = *(_DWORD *)(a1 + 738512);
  }
  *(_DWORD *)(a1 + 738512) = v1 - 1;
  v2 = *(_DWORD *)(a1 + 4 * (v1 - 1) + 710584);
  v8 = (_DWORD *)(a1 + 666392 + 344 * v2);
  v3 = (_DWORD *)*v8;
  if ( *v8 )
  {
    v4 = sub_449120(*v3);
    if ( v4 == -1 )
      v5 = 0;
    else
      v5 = (char *)&unk_4C0240 + 756 * v4;
    PalReleaseFxEffect(v5, 0xFFFFFFF);
    if ( v3[3] )
    {
      PalMemoryFree(v3[3]);
      v3[3] = 0;
    }
    *(_DWORD *)(v3[5] + 16) = v3[4];
    v6 = v3[4];
    if ( v6 )
      *(_DWORD *)(v6 + 20) = v3[5];
    *(_OWORD *)v3 = 0LL;
    *((_QWORD *)v3 + 2) = 0LL;
    *v8 = 0;
  }
  PalDebugPrintf(10, 0, "fx_effect_cls %d", v2);
  return 1;
}
