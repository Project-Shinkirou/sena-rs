// IDA SQL evidence from PAL_d.dll (debug build).
int __cdecl sub_1029F2C0(int a1, int a2, int a3, int a4, int a5, int a6, int a7, int a8, int a9)
{
  _BYTE v10[12]; // [esp+118h] [ebp-18h] BYREF
  int v11; // [esp+124h] [ebp-Ch]

  v11 = *((_DWORD *)dword_283E1CBC + 3);
  (*(void (__thiscall **)(int, int, _DWORD))(*(_DWORD *)a1 + 348))(a1, a1, *((_DWORD *)dword_283E1CBC + 10));
  (*(void (__stdcall **)(int, const char *, int))(*(_DWORD *)v11 + 208))(v11, "g_Texture", a4);
  (*(void (__stdcall **)(int, const char *, int))(*(_DWORD *)v11 + 120))(v11, "g_TextureWidth", a5);
  (*(void (__stdcall **)(int, const char *, int))(*(_DWORD *)v11 + 120))(v11, "g_TextureHeight", a6);
  (*(void (__stdcall **)(int, const char *, int))(*(_DWORD *)v11 + 120))(v11, "offset_x", a7);
  (*(void (__stdcall **)(int, const char *, int))(*(_DWORD *)v11 + 120))(v11, "offset_y", a8);
  (*(void (__stdcall **)(int, const char *, int))(*(_DWORD *)v11 + 120))(v11, "level", a9);
  (*(void (__stdcall **)(int, const char *))(*(_DWORD *)v11 + 236))(v11, "DrawRadialBlur");
  (*(void (__stdcall **)(int, _BYTE *, _DWORD))(*(_DWORD *)v11 + 256))(v11, v10, 0);
  (*(void (__stdcall **)(int, _DWORD))(*(_DWORD *)v11 + 260))(v11, 0);
  (*(void (__thiscall **)(int, int, int, int, int, int))(*(_DWORD *)a1 + 332))(a1, a1, 5, 2, a2, a3);
  (*(void (__stdcall **)(int))(*(_DWORD *)v11 + 264))(v11);
  return 0;
}
