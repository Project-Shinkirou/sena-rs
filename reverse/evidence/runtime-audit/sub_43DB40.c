int __thiscall sub_43DB40(_DWORD *this)
{
  int TaskData; // edi
  int v3; // ecx
  int v4; // ebx
  int v5; // ebx
  int v6; // eax
  int v7; // eax
  int v9; // ecx
  unsigned int v10; // edi
  int v11; // ebx
  _DWORD *v12; // edi
  _DWORD *v13; // esi
  int v14; // eax
  int v15; // esi
  int v16; // ecx
  int v17; // edx
  int v18; // [esp+Ch] [ebp-4h]

  TaskData = PalTaskGetTaskData(0);
  v18 = PalTaskGetTaskData(0) + 824;
  if ( !this[1049] || !this[1045] )
    return 0;
  v4 = sub_44AC00(v3);
  sub_402A20();
  if ( (~(_BYTE)v4 & 4) != 0 && (v4 & 1) != 0 && !*(_DWORD *)(TaskData + 12) && !this[1046] )
    v4 = ~(~v4 | 1);
  v5 = (*(_DWORD *)(PalTaskGetTaskData(0) + 5016) != 0 ? 2 : 0) | v4;
  v6 = this[1049];
  if ( (v6 & 1) != 0 )
  {
    if ( (v6 & 0x40000) != 0 )
    {
      v5 |= 1u;
    }
    else if ( (v6 & 0x20000) != 0 )
    {
      v5 = 0;
    }
    if ( sub_43C120((int)this, v5) )
    {
      v7 = this[1049];
      if ( (v7 & 0x20000) != 0 )
      {
        sub_43B900(this);
      }
      else if ( (v7 & 0x80000) != 0 )
      {
        sub_43B900(this);
        sub_449FC0();
      }
      else
      {
        sub_43A860((int)this);
      }
    }
    if ( (~this[1049] & 0x20000) != 0 )
    {
      sub_439480();
      return 1;
    }
    return 0;
  }
  if ( (v6 & 2) == 0 )
    return 0;
  sub_439480();
  v10 = PaltimeGetTime(v9) - this[1051];
  if ( (!*(_DWORD *)(v18 + 804252) || sub_4428E0(v18 + 655264) || v10 < this[1052]) && !v5 )
    return 0;
  sub_43B900(this);
  sub_449FC0();
  v11 = 16;
  v12 = (_DWORD *)(v18 + 818204);
  v13 = (_DWORD *)(v18 + 818268);
  do
  {
    if ( *v12 )
    {
      if ( *v13 )
        sub_408B00(v13);
      memset(v13, 0, 0x4851Cu);
      *v12 = 0;
    }
    v13 += 74055;
    ++v12;
    --v11;
  }
  while ( v11 );
  v14 = PalTaskGetTaskData(0);
  v15 = v14 + 824;
  if ( byte_54D20D[v14] )
  {
    v16 = *(int *)((char *)&dword_54CEC8 + v15);
    v17 = v16 - 1;
    if ( !v16 )
      v17 = 0;
    if ( v17 >= 0 && *(int *)((char *)&dword_54CED0 + v15) )
    {
      sub_44B7B0(*(_DWORD *)(v14 + 804884), 0);
      *(_DWORD *)(v15 + 804060) = *(int *)((char *)&dword_54CED0 + v15) + *(_DWORD *)(v15 + 804064);
      byte_57D458[v15] = 0;
    }
  }
  return 1;
}