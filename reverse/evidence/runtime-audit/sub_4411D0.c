int __cdecl sub_4411D0(char *a1)
{
  char *v1; // edi
  _DWORD *v3; // edi
  int v4; // ebx
  int *v5; // esi
  int v6; // eax
  int *v7; // ecx
  int v8; // eax
  int v9; // ecx
  int *v10; // esi
  int v11; // eax
  int *v12; // ecx
  int v13; // eax
  int v14; // ecx
  int *v15; // esi
  int v16; // eax
  int *v17; // ecx
  int v18; // eax
  int v19; // ecx
  int v20; // [esp+4h] [ebp-2Ch] BYREF
  int v21; // [esp+8h] [ebp-28h]
  __int128 v22; // [esp+Ch] [ebp-24h]
  __int128 v23; // [esp+1Ch] [ebp-14h]
  int v24; // [esp+2Ch] [ebp-4h]
  _DWORD *v25; // [esp+38h] [ebp+8h]

  v1 = a1 + 4;
  v25 = v1;
  if ( *(_DWORD *)v1 )
  {
    sub_449190(*((void **)v1 + 1066));
    v3 = v1 + 4272;
    v4 = 3;
    do
    {
      v5 = (int *)*(v3 - 1);
      if ( v5 )
      {
        v6 = v5[2];
        if ( v6 )
        {
          v7 = &dword_4BD78C[21 * v6];
          if ( *v7 )
            sub_4471E0(v7);
        }
        v8 = *v5;
        v20 = 196613;
        v21 = v8;
        v22 = 0LL;
        v24 = 0;
        v23 = 0LL;
        sub_44B740(&v20);
        *(_DWORD *)(v5[170] + 684) = v5[171];
        v9 = v5[171];
        if ( v9 )
          *(_DWORD *)(v9 + 680) = v5[170];
        memset(v5, 0, 0x2F4u);
      }
      PalAnimationRelease(*v3);
      v3 += 2;
      --v4;
    }
    while ( v4 );
    v10 = (int *)v25[1063];
    if ( v10 )
    {
      v11 = v10[2];
      if ( v11 )
      {
        v12 = &dword_4BD78C[21 * v11];
        if ( *v12 )
          sub_4471E0(v12);
      }
      v13 = *v10;
      v20 = 196613;
      v21 = v13;
      v22 = 0LL;
      v24 = 0;
      v23 = 0LL;
      sub_44B740(&v20);
      *(_DWORD *)(v10[170] + 684) = v10[171];
      v14 = v10[171];
      if ( v14 )
        *(_DWORD *)(v14 + 680) = v10[170];
      memset(v10, 0, 0x2F4u);
    }
    v15 = (int *)v25[1064];
    if ( v15 )
    {
      v16 = v15[2];
      if ( v16 )
      {
        v17 = &dword_4BD78C[21 * v16];
        if ( *v17 )
          sub_4471E0(v17);
      }
      v18 = *v15;
      v20 = 196613;
      v21 = v18;
      v22 = 0LL;
      v24 = 0;
      v23 = 0LL;
      sub_44B740(&v20);
      *(_DWORD *)(v15[170] + 684) = v15[171];
      v19 = v15[171];
      if ( v19 )
        *(_DWORD *)(v19 + 680) = v15[170];
      memset(v15, 0, 0x2F4u);
    }
    PalMemoryFree(v25[1074]);
    PalMemoryFree(v25[1075]);
    memset(v25, 0, 0x223Cu);
    return 1;
  }
  else
  {
    PalDebugPrintf(0, 0, "text initialize error : %s", "text_uninit");
    return 0;
  }
}
