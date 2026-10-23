// IDA SQL evidence from PAL_d.dll (debug build).
int __cdecl sub_10240D40(int a1, int a2, int a3, void *Src)
{
  int Size; // [esp+D0h] [ebp-14h]

  *(_DWORD *)(a1 + 24) = PalMemoryAlloc(dword_103CB120[a2], 0);
  if ( *(_DWORD *)(a1 + 24) )
  {
    Size = dword_103CB118[a2];
    *(_DWORD *)(a1 + 20) = PalMemoryAlloc(Size, 0);
    if ( *(_DWORD *)(a1 + 20) )
    {
      j__memmove(*(void **)(a1 + 20), Src, Size);
      *(_DWORD *)a1 = 0;
      *(_DWORD *)(a1 + 16) = a2;
      *(_DWORD *)(a1 + 12) = 1;
      *(_DWORD *)(a1 + 8) = a3;
      *(_DWORD *)(a1 + 4) = PaltimeGetTime();
      SetLastError(0);
      return 1;
    }
    else
    {
      if ( *(_DWORD *)(a1 + 24) )
      {
        PalMemoryFree(*(void **)(a1 + 24));
        *(_DWORD *)(a1 + 24) = 0;
      }
      return 0;
    }
  }
  else
  {
    SetLastError(6u);
    return 0;
  }
}
