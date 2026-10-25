int __fastcall sub_43D9D0(int a1, int a2, int a3)
{
  int v5; // edi
  int v6; // ecx
  _BYTE *v7; // edi
  size_t i; // eax
  int result; // eax
  int v10; // eax
  signed int v11; // esi
  _WORD *v12; // ebx
  int v13; // [esp+Ch] [ebp-4h] BYREF

  PalRenderGetActive();
  v5 = **(_DWORD **)(a2 + 4252);
  if ( (sub_44AC00(v6) & 0x30) == 0 )
    PalSpritePaint(v5, 0, 0);
  v7 = (_BYTE *)(a2 + 4344);
  for ( i = 0; v7[i] || v7[i + 1]; i += 2 )
    ;
  memset((void *)(a2 + 4344), 0, i);
  PalDebugPrintf(0, 0, "AdvTextSetName");
  result = sub_43A8F0(a3, (char *)(a2 + 4344));
  if ( result )
  {
    v10 = 2;
    if ( *(_DWORD *)(a2 + 4) == 1 )
      v10 = 0;
    v13 = v10;
    result = sub_43C7C0(
               **(_DWORD **)(a2 + 4252),
               (unsigned __int8 *)(a2 + 4344),
               0,
               &v13,
               1024,
               64,
               *(_DWORD *)(a2 + 12),
               0,
               (_DWORD *)(a2 + 4328),
               0);
    *(_DWORD *)(a2 + 4200) |= 1u;
    if ( !*(_DWORD *)(a1 + 85676) )
    {
      v11 = 0;
      if ( !v7 )
        goto LABEL_17;
      while ( v7[v11] || v7[v11 + 1] )
        v11 += 2;
      if ( v11 >= 4096 )
      {
        PalTaskGetTaskData(0);
        return (int)memmove_0((void *)(a1 + 88452), v7, 0xFFFu);
      }
      else
      {
LABEL_17:
        v12 = (_WORD *)(a1 + 88452);
        PalTaskGetTaskData(0);
        if ( v7 )
        {
          return (int)memmove_0(v12, v7, v11);
        }
        else
        {
          result = 33024;
          *v12 = -32512;
        }
      }
    }
  }
  else if ( !*(_DWORD *)(a1 + 85676) )
  {
    PalTaskGetTaskData(0);
    result = 33024;
    *(_WORD *)(a1 + 88452) = -32512;
  }
  return result;
}