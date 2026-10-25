// Fresh IDA evidence from testcase/dll/Pal.dll, analyzed from a copy.
int __cdecl sub_100B4270(int a1)
{
  _DWORD v2[2]; // [esp+0h] [ebp-10h] BYREF
  _DWORD v3[2]; // [esp+8h] [ebp-8h] BYREF

  if ( Pal3DIs() )
  {
    if ( sub_100B3FE0(&dword_101294BC, v3) )
    {
      sub_100ACA40(a1, v3[0], v3[1]);
    }
    else if ( *((_DWORD *)dword_101294A4 + 191) )
    {
      return 0;
    }
  }
  else
  {
    sub_100B3FE0(&dword_101294BC, v2);
    sub_100A5CB0(a1, v2[0], v2[1]);
  }
  return 1;
}

