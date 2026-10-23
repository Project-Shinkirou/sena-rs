int __cdecl sub_1027AF50(unsigned int *a1, int a2, int a3, int a4)
{
  int i; // [esp+D0h] [ebp-20h]
  int j; // [esp+DCh] [ebp-14h]

  for ( i = 0; i < a3; ++i )
  {
    for ( j = 0; j < a2; ++j )
    {
      if ( HIBYTE(*a1) )
        *a1 = *a1 & 0xFF000000 | ~(*a1 & 0xFFFFFF);
      ++a1;
    }
    a1 += a4 - a2;
  }
  return 1;
}
