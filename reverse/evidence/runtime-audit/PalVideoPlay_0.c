int __cdecl PalVideoPlay_0(char *Source, __int16 a2)
{
  char *v2; // eax
  HWND WindowHandle; // eax
  HWND v5; // eax
  HWND v6; // eax
  HWND v7; // eax
  LPVOID *ppv; // [esp+D0h] [ebp-8h]

  ppv = (LPVOID *)((char *)Block + 12376);
  v2 = (char *)Block + 12376;
  *((_DWORD *)Block + 3108) = 0;
  *((_DWORD *)v2 + 15) = 0;
  ppv[16] = 0;
  ppv[17] = 0;
  *((_WORD *)ppv + 45) = a2;
  if ( *((_DWORD *)Block + 268) && sub_1011C18C(Source, ".wmv") )
  {
    PalFileGetFullPath((char *)ppv + 92, 0x104u, (int)Source);
    *((_WORD *)ppv + 44) = 2;
  }
  else if ( sub_1011C18C(Source, ".mpg") )
  {
    j__strcpy((char *)ppv + 92, Source);
    *((_WORD *)ppv + 44) = 1;
  }
  else
  {
    if ( !sub_1011C18C(Source, ".mp4") )
      return 0;
    j__strcpy((char *)ppv + 92, Source);
    *((_WORD *)ppv + 44) = 3;
  }
  if ( sub_10266CB0(ppv) )
    return 0;
  SetThreadPriority(*((HANDLE *)Block + 78), -2);
  (*(void (__stdcall **)(LPVOID))(*(_DWORD *)ppv[1] + 28))(ppv[1]);
  *((_DWORD *)Block + 5) = 1;
  WindowHandle = (HWND)PalGetWindowHandle();
  PostMessageA(WindowHandle, 0x8065u, 0, 6);
  if ( sub_1011C2D1((int)Block + 480) )
  {
    v5 = (HWND)PalGetWindowHandle();
    PostMessageA(v5, 0x806Eu, 0, 0);
  }
  v6 = (HWND)PalGetWindowHandle();
  ValidateRect(v6, 0);
  v7 = (HWND)PalGetWindowHandle();
  UpdateWindow(v7);
  return 1;
}
