int __fastcall sub_442C10(int *a1, unsigned int a2, char *Str1, __int16 a4, int a5, int a6)
{
  int v9; // ebx
  _DWORD *v10; // edi
  int *v11; // edi
  int i; // ecx
  int v13; // eax
  char v14; // al
  int v15; // edi
  int v16; // esi
  int *v17; // eax
  int v18; // eax
  int v19; // ebx
  int v20; // eax
  __int16 v21; // ax
  int v22; // eax
  signed int *v23; // [esp-4h] [ebp-84h]
  HANDLE hFile; // [esp+14h] [ebp-6Ch]
  DWORD NumberOfBytesRead; // [esp+18h] [ebp-68h] BYREF
  int *v26; // [esp+1Ch] [ebp-64h]
  int *v27; // [esp+20h] [ebp-60h]
  int v28; // [esp+24h] [ebp-5Ch]
  unsigned int v29; // [esp+28h] [ebp-58h]
  char v30[8]; // [esp+2Ch] [ebp-54h] BYREF
  unsigned int v31; // [esp+34h] [ebp-4Ch]
  CHAR Buffer[68]; // [esp+38h] [ebp-48h] BYREF

  v29 = a2;
  a1[3] = 0;
  if ( !*Str1 )
    return 0;
  if ( a6 && (sub_44AC00(a1) & 0x35) != 0 )
  {
    if ( strncmp(Str1, "VO_MITSUYOSHI", MaxCount) || dword_4BD7D8 )
    {
      sub_4420F0(Str1, 1);
      return 1;
    }
    return 0;
  }
  v28 = a4 & 1;
  wsprintfA(Buffer, "%s.mix", Str1);
  hFile = (HANDLE)PalFileCreate(Buffer, v30);
  if ( hFile != (HANDLE)-1 )
  {
    PalDebugPrintf(0, 0, "mix %s", Buffer);
    sub_402A20();
    v9 = 8;
    v26 = a1 + 452;
    v10 = a1 + 452;
    do
    {
      if ( *v10 )
      {
        PalSoundRelease(*v10);
        memset(v10, 0, 0x64u);
      }
      v10 += 25;
      --v9;
    }
    while ( v9 );
    sub_4423A0((int)a1, 0);
    v29 = 0;
    v11 = a1 + 452;
    v27 = a1 + 452;
    while ( 1 )
    {
      ReadFile(hFile, Buffer, 0x40u, &NumberOfBytesRead, 0);
      if ( NumberOfBytesRead != 64 )
        goto LABEL_30;
      PalDebugPrintf(0, 0, "mix %d %d %d %d %d", v9, Buffer[0], Buffer[1], Buffer[2], Buffer[3]);
      for ( i = 0; i < 64; ++i )
        Buffer[i] = Buffer[2 * i];
      PalDebugPrintf(0, 0, "mix %d %s", v9, Buffer);
      v13 = sub_4024D0(Buffer, 1, 0);
      *v11 = v13;
      if ( !v13 )
        break;
      if ( (a4 & 0x40) == 0 )
        sub_4425F0((int)a1, v13, Buffer);
      v14 = a4;
      if ( (a4 & 0x80u) == 0 && a1[1555] )
      {
        v15 = (int)(v11 + 20);
        if ( sub_442550(Buffer, v15, (int)(a1 + 925)) )
        {
          v23 = (signed int *)v15;
          v11 = v27;
          sub_442430((int)a1, *v27, v23);
        }
        else
        {
          v11 = v27;
        }
LABEL_26:
        v14 = a4;
      }
      if ( (v14 & 2) != 0 )
        a1[3] |= 1 << v9;
      v29 += NumberOfBytesRead;
      if ( v29 >= v31 )
      {
LABEL_31:
        CloseHandle(hFile);
        if ( (a4 & 0x100) == 0 && a1[1187] )
          PalSoundSetVolume(
            a1[1093] * (int)(float)((float)((float)(a1[1188] * a1[1189]) * 0.0099999998) * (float)a1[1086]),
            3);
        v16 = 8;
        v17 = v26;
        do
        {
          v18 = *v17;
          if ( v18 )
          {
            if ( (a4 & 4) != 0 )
              PalSoundPlayFade(v18, v28, a5);
            else
              PalSoundPlay(v18, v28);
          }
          v17 = v26 + 25;
          v26 += 25;
          --v16;
        }
        while ( v16 );
        return 2;
      }
LABEL_30:
      ++v9;
      v11 += 25;
      v27 = v11;
      if ( v9 >= 8 )
        goto LABEL_31;
    }
    if ( *(_DWORD *)(PalTaskGetTaskData(0) + 816) )
      PalDebugPrintf(0, 0, "voice file read : %s", Buffer);
    goto LABEL_26;
  }
  v19 = 25 * a2;
  v20 = sub_4024D0(Str1, 1, 0);
  a1[v19 + 452] = v20;
  if ( !v20 )
  {
    if ( *(_DWORD *)(PalTaskGetTaskData(0) + 816) )
      PalDebugPrintf(0, 0, "voice file read : %s", Str1);
    return 0;
  }
  if ( (a4 & 0x40) == 0 )
    sub_4425F0((int)a1, v20, Str1);
  v21 = a4;
  if ( (a4 & 0x80u) == 0 && a1[1555] )
  {
    if ( sub_442550(Str1, (int)&a1[v19 + 472], (int)(a1 + 925)) )
      sub_442430((int)a1, a1[v19 + 452], &a1[v19 + 472]);
    v21 = a4;
  }
  if ( (v21 & 0x100) == 0 && v29 != 8 )
    sub_40B8D0(a1, 1);
  sub_4423A0((int)a1, Str1);
  if ( (a4 & 2) != 0 )
    a1[3] |= 1 << v29;
  v22 = a1[v19 + 452];
  if ( (a4 & 4) != 0 )
    PalSoundPlayFade(v22, v28, a5);
  else
    PalSoundPlay(v22, v28);
  return 2;
}