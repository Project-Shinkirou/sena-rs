// PAL.sqlite (read-only export): 0x1023A740, 0x10239E20, 0x1023AD70.
int __cdecl PalButtonMode_0(unsigned __int8 a1)
{
  int result; // eax

  result = a1;
  *((_DWORD *)Block + 154) = a1;
  return result;
}

int __cdecl sub_10239E20(int a1)
{
  if ( (*(_DWORD *)(a1 + 16) & 1) != 0 )
  {
    if ( (PalInputGetKeyPush() & 0x800000) != 0 )
      return 1;
  }
  else if ( (*(_DWORD *)(a1 + 16) & 2) != 0 )
  {
    if ( (PalInputGetKeyOnEx() & 0x800000) != 0 )
      return 1;
    if ( (PalInputGetKeyPush() & 0x800000) != 0 )
      return 1;
  }
  return 0;
}

int __cdecl PalButtonGetReaction_0(_DWORD *a1, void *a2)
{
  int v3; // eax
  int v4; // eax
  int MouseY; // [esp+DCh] [ebp-2Ch]
  int MouseX; // [esp+E8h] [ebp-20h]
  int *i; // [esp+F4h] [ebp-14h]

  if ( !a1 )
  {
    if ( a2 )
      j__memset(a2, 0, 0xCu);
    return 0;
  }
  if ( !a1[8] )
  {
    MouseX = PalInputGetMouseX();
    MouseY = PalInputGetMouseY();
    j__memmove(a1 + 9, a1 + 6, 0xCu);
    j__memset(a1 + 6, 0, 0xCu);
    a1[7] = sub_1023A070((int)a1, MouseX, MouseY);
    if ( MouseX < 0 || MouseY < 0 )
      a1[7] = 0;
    for ( i = (int *)a1[5]; ; i = (int *)i[5] )
    {
      if ( !i )
        goto LABEL_38;
      if ( i[1] )
      {
        if ( a1[7] == *i )
        {
          if ( !*((_DWORD *)Block + 154) )
          {
            if ( sub_10239E20((int)i) )
            {
              sub_1023A220(i, 2);
              a1[8] = *i;
              PalSoundPlay(a1[18], 0);
              goto LABEL_38;
            }
            if ( a1[10] != a1[7] )
            {
              v3 = sub_10239FD0((int)a1);
              PalSoundPlay(v3, 0);
            }
LABEL_19:
            sub_1023A220(i, 1);
            continue;
          }
          i[4] = 2;
          if ( (*((_DWORD *)Block + 143) & 0x800000) == 0 )
          {
            if ( a1[12] )
            {
              a1[12] = 0;
              sub_1023A220(i, 2);
              a1[8] = *i;
              PalSoundPlay(a1[18], 0);
              goto LABEL_38;
            }
            if ( a1[10] != a1[7] )
            {
              v4 = sub_10239FD0((int)a1);
              PalSoundPlay(v4, 0);
            }
            goto LABEL_19;
          }
          sub_1023A220(i, 2);
          a1[12] = *i;
        }
        else if ( *((_DWORD *)Block + 154) )
        {
          if ( a1[12] == a1[7] )
            sub_1023A220(i, 0);
          else
            a1[12] = 0;
        }
        else
        {
          sub_1023A220(i, 0);
        }
      }
    }
  }
  if ( a1[20] )
  {
    a1[20] = 0;
    PalSoundPlay(a1[18], 0);
  }
  else if ( !PalSoundPlayIs(a1[18]) )
  {
    a1[6] = a1[8];
    a1[8] = 0;
  }
LABEL_38:
  if ( a2 )
    j__memmove(a2, a1 + 6, 0xCu);
  return a1[6];
}

