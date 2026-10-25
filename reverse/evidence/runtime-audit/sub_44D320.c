int __thiscall sub_44D320(_DWORD *lpBuffer)
{
  void *v2; // esi
  void *v4; // esi
  unsigned int v5; // eax
  HWND ActiveWindow; // eax
  int v7; // eax
  void *v8; // esi
  unsigned int v9; // eax
  HWND v10; // eax
  int v11; // eax
  int v12; // [esp-8h] [ebp-30h]
  int v13; // [esp-8h] [ebp-30h]
  unsigned int Buffer; // [esp+10h] [ebp-18h] BYREF
  DWORD NumberOfBytesRead; // [esp+14h] [ebp-14h] BYREF
  _DWORD v16[3]; // [esp+18h] [ebp-10h] BYREF

  *lpBuffer = 0;
  memset(lpBuffer + 12, 0, 0xA139Cu);
  lpBuffer[165110] = 0;
  memset(lpBuffer + 165121, 0, 0x164u);
  memset(lpBuffer + 165211, 0, (size_t)&unk_4DC2AC);
  lpBuffer[1] = 0;
  lpBuffer[4] = 1;
  lpBuffer[3] = 0;
  lpBuffer[2] = 0;
  lpBuffer[5] = 1;
  lpBuffer[8] = 50;
  lpBuffer[7] = 50;
  lpBuffer[9] = 0;
  lpBuffer[6] = -1;
  lpBuffer[10] = -1;
  lpBuffer[11] = -1;
  lpBuffer[165108] = 5000;
  lpBuffer[165107] = 5000;
  lpBuffer[165111] = 5000;
  lpBuffer[165112] = 5000;
  lpBuffer[165113] = 5000;
  lpBuffer[165109] = 5000;
  lpBuffer[165210] = 5000;
  lpBuffer[165114] = 1;
  lpBuffer[165115] = 1;
  lpBuffer[165116] = 1;
  lpBuffer[165117] = 1;
  lpBuffer[165118] = 1;
  lpBuffer[165119] = 1;
  lpBuffer[165120] = 1;
  PalSoundSetPrimaryVolume(5000);
  PalSoundSetVolume(lpBuffer[165109], 1);
  PalSoundSetVolume(lpBuffer[165108], 3);
  PalSoundSetVolume(lpBuffer[165111], 5);
  PalSoundSetVolume(lpBuffer[165112], 4);
  PalSoundSetVolume(lpBuffer[165113], 6);
  v2 = (void *)PalFileCreate("Script.src", v16);
  if ( v2 == (void *)-1 )
  {
    PalErrorLogPrintf(byte_4A3D48);
    return 0;
  }
  else
  {
    PalFileSetFilePointer(v2, v16[0] + 4, 0, 0, 0);
    ReadFile(v2, lpBuffer, 4u, &NumberOfBytesRead, 0);
    CloseHandle(v2);
    v4 = (void *)PalFileCreate("Text.dat", v16);
    if ( v4 == (void *)-1 )
    {
      PalErrorLogPrintf(byte_4A3D74);
      return 0;
    }
    else
    {
      PalFileSetFilePointer(v4, v16[0] + 12, 0, 0, 0);
      ReadFile(v4, &Buffer, 4u, &NumberOfBytesRead, 0);
      CloseHandle(v4);
      v5 = Buffer;
      if ( Buffer > 0x30D40 )
      {
        ActiveWindow = GetActiveWindow();
        MessageBoxA(ActiveWindow, &byte_4A3DA8, &byte_4A3DA0, 0x30u);
        v5 = Buffer;
      }
      v12 = (v5 >> 3) + 1;
      *(int *)((char *)&dword_57D810 + (_DWORD)lpBuffer) = v12;
      v7 = PalMemoryAlloc(v12, 1);
      *(int *)((char *)&dword_57D808 + (_DWORD)lpBuffer) = v7;
      if ( v7 )
      {
        v8 = (void *)PalFileCreate("File.dat", v16);
        if ( v8 == (void *)-1 )
        {
          PalErrorLogPrintf(byte_4A3E24);
          return 0;
        }
        else
        {
          PalFileSetFilePointer(v8, v16[0] + 12, 0, 0, 0);
          ReadFile(v8, &Buffer, 4u, &NumberOfBytesRead, 0);
          CloseHandle(v8);
          v9 = Buffer;
          if ( Buffer >= 0x10000 )
          {
            v10 = GetActiveWindow();
            MessageBoxA(v10, &byte_4A3E50, &byte_4A3DA0, 0x30u);
            v9 = Buffer;
          }
          v13 = (v9 >> 2) + 1;
          *(int *)((char *)&dword_57D80C + (_DWORD)lpBuffer) = v13;
          v11 = PalMemoryAlloc(v13, 1);
          *(int *)((char *)&dword_57D804 + (_DWORD)lpBuffer) = v11;
          if ( v11 )
          {
            sub_44D070(lpBuffer);
            PalErrorLogPrintf(byte_4A3ECC);
            return 1;
          }
          else
          {
            PalErrorLogPrintf(byte_4A3E98);
            return 0;
          }
        }
      }
      else
      {
        PalErrorLogPrintf(byte_4A3DF0);
        return 0;
      }
    }
  }
}
