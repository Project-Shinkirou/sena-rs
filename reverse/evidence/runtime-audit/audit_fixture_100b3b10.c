// Fresh IDA evidence from testcase/dll/Pal.dll, analyzed from a copy.
int __cdecl PalSetFxEffect(int *a1, int a2, int a3, int a4, void *Src)
{
  int v6; // [esp+0h] [ebp-10h]
  int v7; // [esp+8h] [ebp-8h]
  int *v8; // [esp+Ch] [ebp-4h]

  if ( (a2 & 0x10000000) != 0 )
  {
    v8 = &dword_101294BC;
    if ( a3 == 1 )
    {
      if ( dword_101294B8 )
        return 0;
      v6 = *a1;
      dword_101294B8 = PalRenderGetActive();
      PalRenderInsert(dword_101294B8, &dword_101294BC, sub_100B4270, v6);
    }
  }
  else
  {
    v7 = *a1;
    if ( !*(_DWORD *)(*a1 + 28) )
    {
      *(_DWORD *)(v7 + 28) = sub_100B3D60();
      if ( !*(_DWORD *)(v7 + 28) )
      {
        SetLastError(6u);
        return 0;
      }
    }
    v8 = *(int **)(v7 + 28);
  }
  if ( a3 )
  {
    if ( a3 == 1 )
    {
      if ( sub_100B3FC0(v8) )
        return 0;
      if ( !sub_100B3E30((int)v8, a2 & 0xFFFFFFF, a4, Src) )
        return 0;
    }
    else if ( a3 == 2 )
    {
      sub_100B3F20(v8, a4);
    }
  }
  else
  {
    sub_100B3F60(v8);
  }
  return 1;
}

