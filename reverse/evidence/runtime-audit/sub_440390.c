int __cdecl sub_440390(int a1)
{
  void (*v2)(_DWORD, _DWORD, const char *, ...); // edi
  int v3; // ebx
  int v4; // ecx
  int TaskData; // eax
  int v6; // ecx
  int v7; // ecx
  int v8; // ecx
  int v9; // ecx
  int v10; // edx
  bool v11; // zf
  int *v12; // ebx
  int v13; // eax
  int v15; // ecx
  int v16; // eax
  char *v17; // ecx
  int v18; // ebx
  char *v19; // edi
  int v20; // eax
  unsigned int i; // edi
  size_t v22; // eax
  int v23; // eax
  int v24; // ecx
  int v25; // eax
  int v26; // [esp-54h] [ebp-74h]
  int v27; // [esp+Ch] [ebp-14h]
  int v28; // [esp+10h] [ebp-10h]
  int v29; // [esp+18h] [ebp-8h]
  int v30; // [esp+1Ch] [ebp-4h]
  int v31; // [esp+28h] [ebp+8h]

  v2 = (void (*)(_DWORD, _DWORD, const char *, ...))PalDebugPrintf;
  PalDebugPrintf(10, 0, "script_point %08x(%08x)", *(_DWORD *)(a1 + 804060), *(_DWORD *)(a1 + 804224));
  sub_439380((_DWORD *)a1);
  PalDebugPrintf(0, 0, "AdvCommandText");
  v3 = *(_DWORD *)(a1 + 804060) - 12;
  PalDebugPrintf(0, 0, "AdvSetSavePoint");
  if ( !*(_DWORD *)(a1 + 8832) && (~(unsigned __int8)sub_44AC00(v4) & 0x10) != 0 )
  {
    *(_DWORD *)(a1 + 8800) = v3;
    *(_DWORD *)(a1 + 8840) = 0;
    TaskData = PalTaskGetTaskData(0);
    sub_42F850((_DWORD *)(TaskData + 824));
  }
  v6 = *(_DWORD *)(a1 + 738512);
  if ( v6 - 1 < 0 )
  {
    PalDebugPrintf(0, 0, "not stack");
    v6 = *(_DWORD *)(a1 + 738512);
  }
  v7 = v6 - 1;
  *(_DWORD *)(a1 + 738512) = v7;
  v29 = *(_DWORD *)(a1 + 4 * v7 + 710584);
  if ( v7 - 1 < 0 )
  {
    PalDebugPrintf(0, 0, "not stack");
    v7 = *(_DWORD *)(a1 + 738512);
  }
  v8 = v7 - 1;
  *(_DWORD *)(a1 + 738512) = v8;
  v31 = *(_DWORD *)(a1 + 4 * v8 + 710584);
  if ( v8 - 1 < 0 )
  {
    PalDebugPrintf(0, 0, "not stack");
    v8 = *(_DWORD *)(a1 + 738512);
  }
  v9 = v8 - 1;
  *(_DWORD *)(a1 + 738512) = v9;
  v10 = *(_DWORD *)(a1 + 4 * v9 + 710584);
  v30 = v10;
  if ( v9 - 1 < 0 )
  {
    PalDebugPrintf(0, 0, "not stack");
    v9 = *(_DWORD *)(a1 + 738512);
    v10 = v30;
  }
  v11 = *(_DWORD *)(a1 + 4) == 0;
  v12 = (int *)(a1 + 4);
  *(_DWORD *)(a1 + 738512) = v9 - 1;
  v13 = *(_DWORD *)(a1 + 4 * (v9 - 1) + 710584);
  v27 = v13;
  if ( v11 )
  {
    PalDebugPrintf(0, 0, "text initialize error : %s", "text");
    return 0;
  }
  else
  {
    *(_DWORD *)(a1 + 4228) = v29;
    *(_DWORD *)(a1 + 4232) = v31;
    *(_DWORD *)(a1 + 4236) = v10;
    *(_DWORD *)(a1 + 4200) = 65537;
    *(_DWORD *)(a1 + 4204) = 0;
    *(_DWORD *)(a1 + 4196) = v13;
    if ( !sub_439590((_DWORD *)a1, (_DWORD *)(a1 + 4)) )
    {
      memset((void *)(a1 + 4608), 0, 0x1000u);
      PalFontGetColor(a1 + 4340, a1 + 4344);
      v15 = *(_DWORD *)(a1 + 4344);
      *(_DWORD *)(a1 + 4332) = *(_DWORD *)(a1 + 4340);
      *(_DWORD *)(a1 + 4336) = v15;
      if ( !*(_DWORD *)(a1 + 85676) )
      {
        v16 = PalTaskGetTaskData(0);
        v17 = (char *)(a1 + 650808);
        v18 = 127;
        v28 = v16 + 824;
        do
        {
          v19 = v17 - 4428;
          memmove_0(v17, v17 - 4428, 0x114Cu);
          v17 = v19;
          --v18;
        }
        while ( v18 );
        memset((void *)(a1 + 88452), 0, 0x114Cu);
        v12 = (int *)(a1 + 4);
        *(_DWORD *)(a1 + 92876) = *(_DWORD *)(v28 + 804284);
        v2 = (void (*)(_DWORD, _DWORD, const char *, ...))PalDebugPrintf;
      }
      sub_43D9D0(a1, (int)v12, v31);
      sub_43D770(a1, (int)v12, v30);
      sub_43C2C0(a1, (int)v12, v29);
      v2(
        0,
        0,
        "[%d][%d][%d][%d][%d][%d][%d][%d][%d][%d][%d][%d][%d][%d][%d][%d]AdvCommandText0",
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
      if ( !*(_DWORD *)(a1 + 85676) )
      {
        v20 = PalTaskGetTaskData(0);
        *(_DWORD *)(a1 + 92868) = *(_DWORD *)(v20 + 805048);
        *(_DWORD *)(a1 + 92872) = *(_DWORD *)(v20 + 805100);
        if ( (int)++*(_DWORD *)(a1 + 88444) > 128 )
          *(_DWORD *)(a1 + 88444) = 128;
      }
      v2(
        0,
        0,
        "[%d][%d][%d][%d][%d][%d][%d][%d][%d][%d][%d][%d][%d][%d][%d][%d]AdvCommandText1",
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
      for ( i = 0; *((_BYTE *)v12 + i + 4604) || *((_BYTE *)v12 + i + 4605); i += 2 )
        ;
      if ( i <= 0x1000 )
      {
        v22 = lstrlenA((LPCSTR)(a1 + 8860));
        memset((void *)(a1 + 8860), 0, v22);
        memmove_0((void *)(a1 + 8860), v12 + 1151, i + 1);
      }
      PalDebugPrintf(
        0,
        0,
        "[%d][%d][%d][%d][%d][%d][%d][%d][%d][%d][%d][%d][%d][%d][%d][%d]AdvCommandText2",
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
      v23 = v12[1085];
      v26 = v12[1084];
      v12[1082] = v26;
      v12[1083] = v23;
      PalFontSetColor(v26, v23);
      v25 = v27;
      if ( !v27 )
      {
        v24 = PalTaskGetTaskData(0);
        v25 = *(_DWORD *)(v24 + 32) * (v12[1055] / v12[3]);
      }
      v12[1052] = v25;
      v12[1051] = PaltimeGetTime(v24);
      v12[1047] = 0;
      sub_439C10((int)v12, 1);
      PalDebugPrintf(10, 0, "text  %s", (const char *)v12 + 4604);
      sub_44A010();
      if ( v12[1] == 1 )
        sub_439770((int)v12);
      sub_43A6B0(v12);
      *(_DWORD *)(a1 + 804084) = 1;
    }
    return 1;
  }
}