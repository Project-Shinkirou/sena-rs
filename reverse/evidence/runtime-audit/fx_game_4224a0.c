// IDA database Game.sqlite, reference Game.exe, address 0x4224a0.
// Build identity: reverse/evidence/native-interface-inventory.json.
int __cdecl sub_4224A0(int a1)
{
  int v1; // ecx
  int v2; // ecx
  int v3; // ecx
  int v4; // ecx
  int v5; // edi
  int result; // eax
  int *v7; // esi
  int v8; // edi
  int v9; // ecx
  int v10; // edx
  void (*v11)(int, _DWORD, const char *, ...); // edi
  _DWORD *v12; // eax
  int v13; // ecx
  int *v14; // ecx
  float v15; // xmm0_4
  int v16; // [esp-4h] [ebp-38h]
  int v17; // [esp+8h] [ebp-2Ch]
  unsigned int *v18; // [esp+10h] [ebp-24h]
  int v19; // [esp+14h] [ebp-20h]
  int v20; // [esp+18h] [ebp-1Ch]
  int v21; // [esp+1Ch] [ebp-18h]
  int v22; // [esp+20h] [ebp-14h]
  _DWORD v23[3]; // [esp+24h] [ebp-10h] BYREF

  v1 = *(_DWORD *)(a1 + 738512);
  if ( v1 - 1 < 0 )
  {
    PalDebugPrintf(0, 0, "not stack");
    v1 = *(_DWORD *)(a1 + 738512);
  }
  v2 = v1 - 1;
  *(_DWORD *)(a1 + 738512) = v2;
  v22 = *(_DWORD *)(a1 + 4 * v2 + 710584);
  if ( v2 - 1 < 0 )
  {
    PalDebugPrintf(0, 0, "not stack");
    v2 = *(_DWORD *)(a1 + 738512);
  }
  v3 = v2 - 1;
  *(_DWORD *)(a1 + 738512) = v3;
  v19 = *(_DWORD *)(a1 + 4 * v3 + 710584);
  if ( v3 - 1 < 0 )
  {
    PalDebugPrintf(0, 0, "not stack");
    v3 = *(_DWORD *)(a1 + 738512);
  }
  v4 = v3 - 1;
  *(_DWORD *)(a1 + 738512) = v4;
  v20 = *(_DWORD *)(a1 + 4 * v4 + 710584);
  if ( v4 - 1 < 0 )
  {
    PalDebugPrintf(0, 0, "not stack");
    v4 = *(_DWORD *)(a1 + 738512);
  }
  *(_DWORD *)(a1 + 738512) = v4 - 1;
  v5 = *(_DWORD *)(a1 + 4 * (v4 - 1) + 710584);
  v17 = v5;
  v21 = 344 * v22;
  if ( !*(_DWORD *)(344 * v22 + a1 + 666388) )
    return 1;
  result = PalMemoryAlloc(12, 0);
  v18 = (unsigned int *)result;
  if ( result )
  {
    v7 = (int *)&unk_4BD71C;
    *(_DWORD *)result = v19;
    *(_DWORD *)(result + 4) = v20;
    *(_DWORD *)(result + 8) = v5;
    while ( *(v7 - 2) != 2 )
    {
      v11 = (void (*)(int, _DWORD, const char *, ...))PalDebugPrintf;
LABEL_26:
      v7 += 6;
      if ( (int)v7 >= (int)&dword_4BD7DC )
      {
        v14 = (int *)&unk_4BD710;
        while ( v14[1] )
        {
          v14 += 6;
          if ( (int)v14 >= (int)&dword_4BD7D0 )
          {
            v14 = 0;
            break;
          }
        }
        *v14 = v22;
        v14[2] = 0;
        v14[1] = 1;
        v14[3] = (int)v18;
        v14[4] = dword_4BD708;
        v14[5] = (int)&unk_4BD6F8;
        if ( dword_4BD708 )
          *(_DWORD *)(dword_4BD708 + 20) = v14;
        dword_4BD708 = (int)v14;
        *(_DWORD *)(v21 + a1 + 666392) = v14;
        v15 = (float)*v18;
        *(float *)&v23[1] = v15 * 0.0099999998;
        *(float *)&v23[2] = (float)v18[1] * 0.0099999998;
        v23[0] = v18[2];
        PalSetFxEffect(*(_DWORD *)(v21 + a1 + 666388), 0, 1, v17, v23);
        v11(10, 0, "fx_raster_start %d, %d, %d, %d", v22, v19, v20, v17);
        return 1;
      }
    }
    v8 = *(v7 - 3);
    PalTaskGetTaskData(0);
    v9 = (unsigned __int16)v8;
    v10 = v8 & 0xF000000;
    if ( (v8 & 0xF000000) != 0 )
    {
      if ( v10 != 0x1000000 )
      {
        if ( v10 != 0x2000000 )
        {
          v16 = v8;
          v11 = (void (*)(int, _DWORD, const char *, ...))PalDebugPrintf;
          PalDebugPrintf(0, 0, "fail original image no : 0x%08X", v16);
          v12 = 0;
LABEL_16:
          if ( !PalSpriteGetFxEffectState(*v12) )
          {
            *(v7 - 2) = 0;
            if ( *v7 )
            {
              PalMemoryFree(*v7);
              *v7 = 0;
            }
            *(_DWORD *)(v7[2] + 16) = v7[1];
            v13 = v7[1];
            if ( v13 )
              *(_DWORD *)(v13 + 20) = v7[2];
          }
          goto LABEL_26;
        }
        v9 = (((v8 >> 20) & 0xF) << 7) + (unsigned __int16)v8 + 144;
      }
    }
    else
    {
      v9 = (unsigned __int16)v8 + 16;
    }
    v11 = (void (*)(int, _DWORD, const char *, ...))PalDebugPrintf;
    v12 = (_DWORD *)((char *)&unk_4C0240 + 756 * v9);
    goto LABEL_16;
  }
  return result;
}
