// Fresh IDA evidence from testcase/dll/Pal.dll, analyzed from a copy.
int __cdecl sub_100B3E30(int a1, int a2, int a3, void *Src)
{
  int Size; // [esp+0h] [ebp-8h]

  *(_DWORD *)(a1 + 24) = PalMemoryAlloc(dword_1011A150[a2], 0);
  if ( *(_DWORD *)(a1 + 24) )
  {
    Size = dword_1011A148[a2];
    *(_DWORD *)(a1 + 20) = PalMemoryAlloc(Size, 0);
    if ( *(_DWORD *)(a1 + 20) )
    {
      memmove(*(void **)(a1 + 20), Src, Size);
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

