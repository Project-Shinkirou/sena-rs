int __cdecl sub_433D10(int a1)
{
  int v1; // ecx
  int v2; // ecx
  unsigned int v3; // edi
  int v4; // ecx
  bool v5; // zf
  int TaskData; // [esp+Ch] [ebp-210h]
  CHAR v8[520]; // [esp+10h] [ebp-20Ch] BYREF

  TaskData = PalTaskGetTaskData(0);
  PalDebugPrintf(
    0,
    0,
    "[%d][%d][%d][%d][%d][%d][%d][%d][%d][%d][%d][%d][%d][%d][%d][%d]AdvCommandSave",
    *(char *)(a1 + 8860),
    *(char *)(a1 + 8861),
    *(char *)(a1 + 8862),
    *(char *)(a1 + 8863),
    *(char *)(a1 + 8864),
    *(char *)(a1 + 8865),
    *(char *)(a1 + 8866),
    *(char *)(a1 + 8867),
    *(char *)(a1 + 8868),
    *(char *)(a1 + 8869),
    *(char *)(a1 + 8870),
    *(char *)(a1 + 8871),
    *(char *)(a1 + 8872),
    *(char *)(a1 + 8873),
    *(char *)(a1 + 8874),
    *(char *)(a1 + 8875));
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
  v4 = v2 - 1;
  v5 = *(_DWORD *)(a1 + 8828) == 0;
  *(_DWORD *)(a1 + 738512) = v4;
  if ( !v5 )
  {
    if ( v3 > 0x3E7 )
    {
      PalDebugPrintf(0, 0, "error : save file no [%d]", v3);
    }
    else
    {
      if ( *(_DWORD *)(a1 + 4 * v4 + 710584) )
        *(_DWORD *)(TaskData + 24) = v3;
      wsprintfA(v8, "%s\\save%03d.dat", DirectoryName, v3);
      sub_430760(v8);
      PalDebugPrintf(10, 0, "save  %s", v8);
    }
  }
  return 1;
}
