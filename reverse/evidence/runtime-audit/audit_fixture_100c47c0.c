// Fresh IDA evidence from testcase/dll/Pal.dll, analyzed from a copy.
int __cdecl PalSpriteSetFxEffect(int a1, int a2, int a3, int a4, void *Src)
{
  if ( !*(_DWORD *)(a1 + 28) )
  {
    *(_DWORD *)(a1 + 28) = sub_100B3D60();
    if ( !*(_DWORD *)(a1 + 28) )
    {
      SetLastError(6u);
      return 0;
    }
  }
  if ( a3 )
  {
    if ( a3 == 1 )
    {
      if ( sub_100B3FC0(*(_DWORD *)(a1 + 28)) )
        return 0;
      if ( !sub_100B3E30(*(_DWORD *)(a1 + 28), a2, a4, Src) )
        return 0;
    }
    else if ( a3 == 2 )
    {
      sub_100B3F20(*(_DWORD *)(a1 + 28), a4);
    }
  }
  else
  {
    sub_100B3F60(*(_DWORD *)(a1 + 28));
  }
  return 1;
}

