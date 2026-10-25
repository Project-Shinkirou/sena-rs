int __cdecl sub_431130(int a1)
{
  int v1; // eax
  int v2; // eax
  int v3; // edi
  int TaskData; // [esp+8h] [ebp-210h]
  CHAR FileName[520]; // [esp+Ch] [ebp-20Ch] BYREF

  TaskData = PalTaskGetTaskData(0);
  v1 = *(_DWORD *)(a1 + 738512);
  if ( v1 - 1 < 0 )
  {
    PalDebugPrintf(0, 0, "not stack");
    v1 = *(_DWORD *)(a1 + 738512);
  }
  v2 = v1 - 1;
  *(_DWORD *)(a1 + 738512) = v2;
  v3 = *(_DWORD *)(a1 + 4 * v2 + 710584);
  if ( v3 == -1 )
    wsprintfA(FileName, "%s\\%s", DirectoryName, "continue.dat");
  else
    wsprintfA(FileName, "%s\\save%03d.dat", DirectoryName, v3);
  *(_DWORD *)(a1 + 4 * *(_DWORD *)(a1 + 738516) + 710840) = DeleteFileA(FileName);
  if ( v3 == *(_DWORD *)(TaskData + 24) )
    *(_DWORD *)(TaskData + 24) = -1;
  PalDebugPrintf(10, 0, "delete_file %s", FileName);
  return 1;
}
