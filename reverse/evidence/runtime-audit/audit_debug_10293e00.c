// IDA SQL evidence from PAL_d.dll (debug build).
int __cdecl sub_10293E00(int a1)
{
  const CHAR *v2; // eax
  int v3[3]; // [esp+D0h] [ebp-24h] BYREF
  HMODULE hLibModule; // [esp+DCh] [ebp-18h]
  int v5; // [esp+E8h] [ebp-Ch]

  hLibModule = 0;
  if ( *(_DWORD *)(a1 + 796) == 3 )
  {
    hLibModule = LoadLibraryA("dll\\FX.dll");
    if ( !hLibModule )
      return sub_1011F99A("Load Library Error : FX.dll \n", 0);
    v5 = sub_10120C32(*(_DWORD *)(a1 + 8), hLibModule, "BICUBIC", 0, 0, 4, 0, a1 + 44, (int)v3);
    if ( v5 < 0 )
    {
      v2 = (const CHAR *)(*(int (__stdcall **)(int))(*(_DWORD *)v3[0] + 12))(v3[0]);
      MessageBoxA(0, v2, "Error", 0);
      FreeLibrary(hLibModule);
      return sub_1011F99A("Load Library Error : FX.dll \n", 0);
    }
    FreeLibrary(hLibModule);
  }
  return 1;
}
