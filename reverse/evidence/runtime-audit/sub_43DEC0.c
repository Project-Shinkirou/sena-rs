int __cdecl sub_43DEC0(_DWORD *a1)
{
  int v1; // ecx
  _DWORD *v2; // edi
  int v3; // eax
  int *v4; // ecx
  int Active; // eax
  int v6; // ecx
  int v7; // eax
  _DWORD *v8; // ebx
  int v9; // edi
  int v10; // eax
  int result; // eax
  int v12; // [esp+Ch] [ebp-14h]
  _DWORD v13[3]; // [esp+10h] [ebp-10h]

  v13[0] = 0;
  v13[1] = 1;
  v1 = a1[184628];
  v13[2] = 2;
  if ( v1 - 1 < 0 )
  {
    PalDebugPrintf(0, 0, "not stack");
    v1 = a1[184628];
  }
  a1[184628] = v1 - 1;
  v2 = (_DWORD *)a1[1066];
  v12 = a1[v1 + 177645];
  a1[2] = v13[v12];
  if ( v2 )
  {
    v3 = v2[2];
    if ( v3 )
    {
      v4 = &dword_4BD78C[21 * v3];
      if ( *v4 )
        sub_4471E0(v4);
    }
    Active = PalRenderGetActive();
    PalSpriteRelease(Active);
    *(_DWORD *)(v2[170] + 684) = v2[171];
    v6 = v2[171];
    if ( v6 )
      *(_DWORD *)(v6 + 680) = v2[170];
    memset(v2, 0, 0x2F4u);
  }
  v7 = a1[2];
  v8 = a1 + 13;
  a1[1066] = 0;
  if ( !v7 || v7 == 2 )
  {
    memset(a1 + 13, 0, 0x1024u);
    *(_DWORD *)(a1[1064] + 16) = a1[10];
    *(_DWORD *)(a1[1064] + 20) = a1[11];
    *(float *)(a1[1064] + 108) = (float)(int)a1[10];
    *(float *)(a1[1064] + 112) = (float)(int)a1[11];
    *(_DWORD *)(a1[1065] + 16) = a1[6];
    *(_DWORD *)(a1[1065] + 20) = a1[7];
    *(float *)(a1[1065] + 108) = (float)(int)a1[6];
    *(float *)(a1[1065] + 112) = (float)(int)a1[7];
  }
  else if ( v7 == 1 )
  {
    v9 = sub_4492A0(0, 6);
    v10 = PalRenderGetActive();
    result = PalSpriteCreate(v10, 1920, 1080, 0);
    *(_DWORD *)v9 = result;
    if ( !result )
      return result;
    *(_DWORD *)(v9 + 16) = *v8;
    *(_DWORD *)(v9 + 20) = a1[14];
    *(float *)(v9 + 108) = (float)(int)*v8;
    *(float *)(v9 + 112) = (float)(int)a1[14];
    a1[1066] = v9;
  }
  PalDebugPrintf(10, 0, "set_text_mode %d", v12);
  return 1;
}
