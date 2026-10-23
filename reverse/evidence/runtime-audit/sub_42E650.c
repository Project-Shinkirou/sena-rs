int __cdecl sub_42E650(int a1)
{
  int v1; // ecx
  int v2; // ecx
  int v3; // esi
  const char *v4; // eax
  int v5; // esi
  int v7; // [esp+Ch] [ebp-14Ch]
  CHAR v8[260]; // [esp+10h] [ebp-148h] BYREF
  char Str1[64]; // [esp+114h] [ebp-44h] BYREF

  v1 = *(_DWORD *)(a1 + 738512);
  if ( v1 - 1 < 0 )
  {
    PalDebugPrintf(0, 0, "not stack");
    v1 = *(_DWORD *)(a1 + 738512);
  }
  v2 = v1 - 1;
  *(_DWORD *)(a1 + 738512) = v2;
  v3 = *(_DWORD *)(a1 + 4 * v2 + 710584);
  if ( v2 - 1 < 0 )
  {
    PalDebugPrintf(0, 0, "not stack");
    v2 = *(_DWORD *)(a1 + 738512);
  }
  *(_DWORD *)(a1 + 738512) = v2 - 1;
  v7 = *(unsigned __int16 *)(a1 + 4 * (v2 - 1) + 710584);
  if ( (sub_44AC00(v2) & 0x30) == 0 )
  {
    sub_44B1E0(Str1, v3, 1);
    v4 = ".wmv";
    v5 = 0;
    if ( ".wmv" )
    {
      while ( 1 )
      {
        wsprintfA(v8, "%s%s", Str1, v4);
        if ( PalVideoPlay(v8, v7) )
          break;
        v4 = off_49C3D4[v5++];
        if ( !v4 )
          goto LABEL_9;
      }
      PalDebugPrintf(10, 0, "movie_play  %s", v8);
    }
    else
    {
LABEL_9:
      PalDebugPrintf(0, 0, "error movie_play [%s]", Str1);
    }
    *(_DWORD *)(a1 + 804084) = 1;
  }
  return 1;
}
