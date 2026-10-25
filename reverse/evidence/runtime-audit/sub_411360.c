int __cdecl sub_411360(_DWORD *a1)
{
  int v1; // ecx
  int v2; // ecx
  int v3; // ebx
  int v4; // ecx
  char v5; // al
  HWND WindowHandle; // eax
  int v7; // ebx
  int TaskData; // eax
  int v9; // eax
  int v11; // [esp+Ch] [ebp-314h]
  CHAR String[4]; // [esp+10h] [ebp-310h] BYREF
  _BYTE v13[516]; // [esp+14h] [ebp-30Ch] BYREF
  char v14[260]; // [esp+218h] [ebp-108h] BYREF

  v1 = a1[184628];
  if ( v1 - 1 < 0 )
  {
    PalDebugPrintf(0, 0, "not stack");
    v1 = a1[184628];
  }
  v2 = v1 - 1;
  a1[184628] = v2;
  v3 = a1[v2 + 177646];
  if ( v2 - 1 < 0 )
  {
    PalDebugPrintf(0, 0, "not stack");
    v2 = a1[184628];
  }
  a1[184628] = v2 - 1;
  v11 = a1[v2 + 177645];
  if ( PalDebugIs() )
  {
    if ( v3 == 0xFFFFFFF )
    {
      wsprintfA(String, "%s", ClassName);
    }
    else
    {
      sub_44B120(String, v3, 0x104u);
      sub_43D350(v14, v13);
      v4 = 0;
      do
      {
        v5 = v14[v4++];
        byte_4BD5DB[v4] = v5;
      }
      while ( v5 );
      PalDebugPrintf(16, 0, "-- %s --", v14);
      PalDebugPrintf(0, 0, "-- %s --", v14);
      if ( v11 == 1 )
        wsprintfA(String, "%s", v14);
      else
        wsprintfA(String, "%s - %s", ClassName, v14);
    }
    WindowHandle = (HWND)PalGetWindowHandle();
    SetWindowTextA(WindowHandle, String);
    PalDebugPrintf(10, 0, "set_window_text %s", String);
  }
  v7 = a1[201015];
  PalDebugPrintf(0, 0, "AdvSetSavePoint");
  if ( !a1[2208] && (~(unsigned __int8)sub_44AC00() & 0x10) != 0 )
  {
    a1[2200] = v7;
    a1[2210] = 0;
    TaskData = PalTaskGetTaskData(0);
    sub_42F850(TaskData + 824);
  }
  v9 = PalTaskGetTaskData(0);
  *(_DWORD *)(v9 + 805100) = *(_DWORD *)(v9 + 804884);
  return 1;
}