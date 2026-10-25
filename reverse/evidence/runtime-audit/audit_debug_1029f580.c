// IDA SQL evidence from PAL_d.dll (debug build).
int __cdecl sub_1029F580(int a1, int a2, int a3, int a4, int a5, int a6, int a7)
{
  _BYTE v8[12]; // [esp+100h] [ebp-18h] BYREF
  int v9; // [esp+10Ch] [ebp-Ch]

  v9 = *((_DWORD *)dword_283E1CBC + 2);
  (*(void (__thiscall **)(int, int, _DWORD))(*(_DWORD *)a1 + 348))(a1, a1, *((_DWORD *)dword_283E1CBC + 10));
  (*(void (__stdcall **)(int, const char *, int))(*(_DWORD *)v9 + 208))(v9, "g_Texture", a4);
  (*(void (__stdcall **)(int, const char *, int))(*(_DWORD *)v9 + 120))(v9, "offset", a5);
  (*(void (__stdcall **)(int, const char *, int))(*(_DWORD *)v9 + 120))(v9, "undulate_x", a6);
  (*(void (__stdcall **)(int, const char *, int))(*(_DWORD *)v9 + 120))(v9, "undulate_y", a7);
  (*(void (__stdcall **)(int, const char *))(*(_DWORD *)v9 + 236))(v9, "DrawRaster");
  (*(void (__stdcall **)(int, _BYTE *, _DWORD))(*(_DWORD *)v9 + 256))(v9, v8, 0);
  (*(void (__stdcall **)(int, _DWORD))(*(_DWORD *)v9 + 260))(v9, 0);
  (*(void (__thiscall **)(int, int, int, int, int, int))(*(_DWORD *)a1 + 332))(a1, a1, 5, 2, a2, a3);
  (*(void (__stdcall **)(int))(*(_DWORD *)v9 + 264))(v9);
  return 0;
}
