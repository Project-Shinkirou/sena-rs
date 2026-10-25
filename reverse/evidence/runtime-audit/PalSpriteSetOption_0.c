int __cdecl PalSpriteSetOption_0(int *a1, unsigned int a2)
{
  if ( !a1 || !*a1 )
  {
    SetLastError(6u);
    return 0;
  }
  a1[33] = a2;
  if ( a2 )
  {
    if ( !a1[1] )
      sub_1011831B((int)a1, a1 + 1);
    if ( Pal3DIs() )
    {
      PalDebugPrintf(0, 0, (int)"[%d][%d]dwType", *a1);
      if ( a2 > 0x105 )
      {
        if ( a2 > 0x605 )
        {
          if ( a2 != 1797 && a2 != 2053 )
          {
            if ( a2 == 2309 )
            {
              PalDebugPrintf(0, 0, (int)"[%d][%d][%d]Pal3DLayerDirectGaussianBlur", *a1);
              sub_10119CD4(*a1, 1);
            }
            return 1;
          }
        }
        else if ( a2 != 1541 )
        {
          if ( a2 > 0x405 )
          {
            if ( a2 != 1285 )
              return 1;
          }
          else if ( a2 != 1029 && a2 != 517 && a2 != 773 )
          {
            return 1;
          }
        }
      }
      else if ( a2 != 261 )
      {
        switch ( a2 )
        {
          case 1u:
            sub_10120020(*a1, (int)&unk_103CC938);
            break;
          case 2u:
            sub_10120020(*a1, 0);
            break;
          case 3u:
            sub_10121533(*a1);
            break;
          case 4u:
            sub_10118109(*a1);
            break;
          default:
            return 1;
        }
        return 1;
      }
      PalDebugPrintf(0, 0, (int)"[%d][%d][%d]Pal3DLayerGaussianBlur", *a1);
      sub_1011FA85(*a1, (a2 >> 8) - 1);
    }
    else
    {
      if ( a2 > 0x105 )
      {
        if ( a2 > 0x505 )
        {
          if ( a2 != 1541 && a2 != 1797 && a2 != 2053 )
            return 1;
        }
        else if ( a2 != 1285 && a2 != 517 && a2 != 773 && a2 != 1029 )
        {
          return 1;
        }
      }
      else if ( a2 != 261 )
      {
        switch ( a2 )
        {
          case 1u:
            sub_1011D442(*a1, (int)&unk_103CC938);
            break;
          case 2u:
            sub_1011D442(*a1, 0);
            break;
          case 3u:
            sub_1011A6AC(*a1);
            break;
          case 4u:
            sub_1011F972(*a1);
            break;
          default:
            return 1;
        }
        return 1;
      }
      sub_1011AF80(*a1, (a2 >> 8) - 1);
    }
  }
  else
  {
    sub_1011EAE5((int)a1, (int)(a1 + 1));
    sub_1011C5DD(a1 + 1);
  }
  return 1;
}
