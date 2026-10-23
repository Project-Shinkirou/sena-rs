int __cdecl sub_4433A0(int a1)
{
  int v1; // ecx
  int v2; // ecx
  int v3; // ebx
  int v4; // ecx
  CHAR *v6; // ecx
  CHAR v7; // al
  int v8; // [esp+Ch] [ebp-48h]
  CHAR String[64]; // [esp+10h] [ebp-44h] BYREF

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
  v4 = *(_DWORD *)(a1 + 4 * (v2 - 1) + 710584);
  if ( v3 >= 0 )
  {
    v8 = a1 + 124 * v3;
    sub_44B1E0(String, v4, 0);
    if ( lstrlenA(String) <= 32 )
    {
      v6 = String;
      do
      {
        v7 = *v6++;
        v6[v8 + 658064 - (_DWORD)String - 1] = v7;
      }
      while ( v7 );
      *(_DWORD *)(v8 + 658060) = 1;
      PalDebugPrintf(10, 0, "set_bgv_auto_volume %d, %s", v3, (const char *)(v8 + 658064));
      return 1;
    }
    else
    {
      PalDebugPrintf(0, 0, "set_bgv_auto_volume_key : size over[%s]", String);
      return 0;
    }
  }
  else
  {
    PalDebugPrintf(0, 0, "set_bgv_auto_volume error : failed ch %d", v3);
    return 0;
  }
}