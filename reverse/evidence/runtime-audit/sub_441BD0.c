int __cdecl sub_441BD0(_DWORD *a1)
{
  void (*v1)(_DWORD, _DWORD, const char *, ...); // edx
  int v3; // ecx
  int v4; // ecx
  int v5; // ebx
  int v6; // eax
  int v7; // ebx
  char *v8; // esi
  int v10; // [esp+Ch] [ebp-4h]
  int v11; // [esp+18h] [ebp+8h]

  v1 = (void (*)(_DWORD, _DWORD, const char *, ...))PalDebugPrintf;
  v3 = a1[184628];
  if ( v3 - 1 < 0 )
  {
    PalDebugPrintf(0, 0, "not stack");
    v3 = a1[184628];
    v1 = (void (*)(_DWORD, _DWORD, const char *, ...))PalDebugPrintf;
  }
  v4 = v3 - 1;
  a1[184628] = v4;
  v5 = a1[v4 + 177646];
  v10 = v5;
  if ( v4 - 1 < 0 )
  {
    v1(0, 0, "not stack");
    v4 = a1[184628];
  }
  a1[184628] = v4 - 1;
  v11 = a1[v4 + 177645];
  v6 = 6188 * v5;
  v7 = 0;
  v8 = (char *)dword_54CED8 + (_DWORD)a1 + v6;
  switch ( v11 )
  {
    case 0:
      v7 = *(_DWORD *)v8;
      break;
    case 1:
      v7 = *((_DWORD *)v8 + 1);
      break;
    case 2:
      v7 = *((_DWORD *)v8 + 2);
      break;
    case 3:
      v7 = *((_DWORD *)v8 + 3);
      break;
    case 4:
      v7 = *((_DWORD *)v8 + 4);
      break;
    case 5:
      v7 = *((_DWORD *)v8 + 5);
      break;
    case 6:
      v7 = *((_DWORD *)v8 + 6);
      break;
    case 7:
      v7 = *((_DWORD *)v8 + 7);
      break;
    case 8:
      v7 = *((_DWORD *)v8 + 8);
      break;
    case 9:
      v7 = v8[44] != 0;
      break;
    case 10:
      switch ( (unsigned __int16)PalFontGetEffect() )
      {
        case 0u:
          v7 = *((_DWORD *)v8 + 9);
          break;
        case 1u:
        case 2u:
          v7 = *((_DWORD *)v8 + 9) + 2;
          break;
        case 3u:
          v7 = *((_DWORD *)v8 + 9) + 4;
          break;
        default:
          goto LABEL_30;
      }
      break;
    case 11:
      switch ( (unsigned __int16)PalFontGetEffect() )
      {
        case 0u:
          v7 = *((_DWORD *)v8 + 10);
          break;
        case 1u:
        case 2u:
          v7 = *((_DWORD *)v8 + 10) + 2;
          break;
        case 3u:
          v7 = *((_DWORD *)v8 + 10) + 4;
          break;
        default:
          goto LABEL_30;
      }
      break;
    case 12:
      v7 = a1[4];
      break;
    case 13:
      v7 = a1[6];
      break;
    case 14:
      v7 = a1[7];
      break;
    case 15:
      v7 = a1[10];
      break;
    case 16:
      v7 = a1[11];
      break;
    case 17:
      v7 = a1[5];
      break;
    default:
      break;
  }
LABEL_30:
  PalDebugPrintf(10, 0, "tips_get_param %d %d", v10, v11);
  a1[a1[184629] + 177710] = v7;
  return 1;
}