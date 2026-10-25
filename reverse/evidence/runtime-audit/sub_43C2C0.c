int __fastcall sub_43C2C0(int a1, int a2, int a3)
{
  int v4; // esi
  int result; // eax
  _DWORD *v6; // esi
  int v7; // edi
  void (__cdecl *v8)(_DWORD); // ebx
  int v9; // edi
  int v10; // eax
  int v11; // ebx
  bool v12; // zf
  signed int v13; // esi
  _DWORD *v14; // esi
  int v15; // edi
  int v16; // [esp+Ch] [ebp-54h]
  char *Destination; // [esp+14h] [ebp-4Ch]
  int v20; // [esp+18h] [ebp-48h]
  CHAR String[64]; // [esp+1Ch] [ebp-44h] BYREF

  v4 = a1;
  memset(String, 0, sizeof(String));
  Destination = (char *)(a2 + 8700);
  memset((void *)(a2 + 8700), 0, 0x40u);
  result = a3;
  if ( a3 != 0xFFFFFFF )
  {
    sub_44B1E0(String, a3, 1);
    result = strncmp(String, "VO_MITSUYOSHI", MaxCount);
    if ( result || dword_4BD7D8 )
    {
      PalDebugPrintf(0, 0, "text : voice %s", String);
      v16 = v4 + 655264;
      sub_402A20();
      v6 = (_DWORD *)(v4 + 657072);
      v7 = 8;
      do
      {
        if ( *v6 )
        {
          PalSoundRelease(*v6);
          memset(v6, 0, 0x64u);
        }
        v6 += 25;
        --v7;
      }
      while ( v7 );
      sub_4423A0(v16, 0);
      result = sub_442C10(String, 0, 0, 1);
      if ( result )
      {
        strncpy(Destination, String, 0x40u);
        v8 = (void (__cdecl *)(_DWORD))PalTaskGetTaskData;
        v9 = PalTaskGetTaskData(0) + 824;
        v10 = sub_402930((const char *)v9);
        if ( v10 != -1 )
        {
          v11 = v9 + 44 * v10;
          v12 = *(_DWORD *)(v11 + 659664) == 0;
          *(_DWORD *)(v11 + 659660) = 0;
          if ( !v12 )
          {
            *(_DWORD *)(v11 + 659660) = 1;
            sub_44B7B0(*(_DWORD *)(v9 + 804060), 2);
            *(_DWORD *)(v9 + 804060) = *(_DWORD *)(v11 + 659664) + *(_DWORD *)(v9 + 804064);
            sub_44A2F0(v9 + 804052);
          }
          v8 = (void (__cdecl *)(_DWORD))PalTaskGetTaskData;
        }
        if ( !*(_DWORD *)(a1 + 85676) )
        {
          v13 = lstrlenA(String);
          if ( v13 >= 4096 )
            v13 = 4095;
          v8(0);
          strncpy((char *)(a1 + 92803), String, v13);
        }
        PalDebugPrintf(10, 0, "text : voice %s", String);
        strncpy(Destination, String, 0x40u);
        result = a2;
        *(_DWORD *)(a2 + 4200) |= 2u;
        return result;
      }
      v4 = a1;
    }
  }
  if ( *(_DWORD *)(v4 + 660024) )
  {
    v20 = v4 + 655264;
    sub_402A20();
    v14 = (_DWORD *)(v4 + 657072);
    v15 = 8;
    do
    {
      if ( *v14 )
      {
        PalSoundRelease(*v14);
        memset(v14, 0, 0x64u);
      }
      v14 += 25;
      --v15;
    }
    while ( v15 );
    return sub_4423A0(v20, 0);
  }
  return result;
}