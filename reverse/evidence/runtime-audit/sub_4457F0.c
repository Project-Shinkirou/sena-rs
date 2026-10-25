int __thiscall sub_4457F0(_DWORD *this)
{
  int v1; // esi
  int Key; // ebx
  int TaskData; // eax
  BOOL v4; // ebx
  int v5; // eax
  int result; // eax
  int i; // esi
  int (__cdecl *v8)(_DWORD); // ecx
  _DWORD *v9; // esi
  int v10; // edi
  int v11; // edi
  _DWORD *v12; // esi
  int v13; // ebx
  _DWORD *j; // esi
  int v15; // ecx
  _DWORD *v16; // esi
  int v17; // edi
  int k; // edi
  _DWORD *v19; // esi
  int v20; // ebx
  int v21; // [esp+Ch] [ebp-8h]
  int v22; // [esp+Ch] [ebp-8h]

  v1 = PalTaskGetTaskData(0) + 824;
  Key = PalInputGetKeyEx();
  v21 = Key;
  if ( Key )
  {
    TaskData = PalTaskGetTaskData(0);
    if ( (Key & 8) != 0 )
    {
      v4 = *(_DWORD *)(TaskData + 8) == 0;
      v5 = PalTaskGetTaskData(0);
      *(int *)((char *)&dword_54D1C4 + v5) = v4;
      Key = v21;
      *(int *)((char *)&dword_54D1C0 + v5) = 1;
      *(int *)((char *)&dword_54D1B8 + v5) = 1;
    }
  }
  if ( *(int *)((char *)&dword_54CE84 + v1) && *(int *)((char *)&dword_54CE80 + v1) )
  {
    sub_44B7B0(*(_DWORD *)(v1 + 804060), 2);
    *(_DWORD *)(v1 + 804060) = *(int *)((char *)&dword_54CE84 + v1) + *(_DWORD *)(v1 + 804064);
    PalDebugPrintf(0, 0, "[%d]AdvMessageProc", Key);
    *(int *)((char *)&dword_54CE80 + v1) = 0;
    return sub_44A2F0(this + 201013);
  }
  result = IsMovie();
  if ( !result )
  {
    if ( sub_44AC00() )
      PalFrameRateSet(-1);
    else
      PalFrameRateSet(60);
    this[201061] |= PalInputGetKeyPush();
    for ( i = dword_653398; i; i = *(_DWORD *)(i + 684) )
      memset((void *)(i + 156), 0, 0x90u);
    sub_445560(this);
    v8 = PalSoundPlayIs;
    v9 = this + 163822;
    v10 = 16;
    do
    {
      if ( *v9 && (v9[2] & 0x20000000) == 0 )
      {
        if ( !v8(*v9) )
        {
          PalSoundRelease(*v9);
          memset(v9, 0, 0x60u);
        }
        v8 = PalSoundPlayIs;
      }
      v9 += 24;
      --v10;
    }
    while ( v10 );
    v11 = 2;
    v12 = this + 164206;
    do
    {
      if ( *v12 && (v12[2] & 0x20000000) == 0 )
      {
        if ( v8(*v12) )
        {
          if ( (v12[2] & 0x40020000) != 0 )
            sub_40B840(v12);
        }
        else
        {
          PalSoundRelease(*v12);
          memset(v12, 0, 0x7Cu);
        }
      }
      v8 = PalSoundPlayIs;
      v12 += 31;
      --v11;
    }
    while ( v11 );
    v22 = 1;
    v13 = 0;
    for ( j = this + 164268; ; j += 25 )
    {
      if ( *j )
      {
        if ( PalSoundPlayIs(*j) )
        {
          v15 = 0;
          v22 = 0;
          if ( !this[165371] || j[23] != 2 )
            goto LABEL_38;
          sub_442430(j + 20);
        }
        else
        {
          if ( v13 != 8 )
            sub_4423A0(this + 163816, 0);
          PalSoundRelease(*j);
          *j = 0;
        }
      }
      v15 = v22;
LABEL_38:
      if ( ++v13 >= 9 )
      {
        if ( v15 && this[165003] )
          PalSoundSetVolume(this[164902] * this[164909], 3);
        v16 = this + 164493;
        v17 = 8;
        do
        {
          if ( *v16 && !PalSoundPlayIs(*v16) && !v16[22] && !(v16[21] | v16[2] & 1) )
          {
            PalSoundRelease(*v16);
            memset(v16, 0, 0x50u);
          }
          v16 += 31;
          --v17;
        }
        while ( v17 );
        sub_408DF0(this + 204550);
        for ( k = dword_4C0230; k; k = *(_DWORD *)(k + 80) )
        {
          v19 = (_DWORD *)(k + 44);
          v20 = 8;
          do
          {
            if ( *v19 && !sub_447040(*v19) )
            {
              PalMemoryFree(*(_DWORD *)(*v19 + 16));
              PalMemoryFree(*v19);
              *v19 = 0;
            }
            ++v19;
            --v20;
          }
          while ( v20 );
          if ( !sub_447040(k) )
          {
            k = *(_DWORD *)(k + 76);
            sub_4471E0(*(void **)(k + 80));
          }
        }
        sub_4240F0(this + 166597);
        return sub_4498D0();
      }
    }
  }
  return result;
}