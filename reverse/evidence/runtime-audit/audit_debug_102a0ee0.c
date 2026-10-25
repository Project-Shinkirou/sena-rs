// IDA SQL evidence from PAL_d.dll (debug build).
int __cdecl sub_102A0EE0(int a1, HMODULE hLibModule)
{
  int v3; // [esp+D0h] [ebp-8h]

  v3 = sub_1011AA49(a1, hLibModule, (LPCWSTR)"RADIAL_BLUR", 2);
  if ( v3 >= 0 )
  {
    *(_DWORD *)dword_283E1CBC |= 4u;
    return v3;
  }
  else
  {
    FreeLibrary(hLibModule);
    return sub_1011F99A("Load Effect Error : EGL3D_EFFECT_TYPE_RADIAL_BLUE", v3);
  }
}
