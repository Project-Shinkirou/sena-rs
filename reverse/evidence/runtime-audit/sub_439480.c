int sub_439480()
{
  int result; // eax
  int v1; // esi
  int v2; // ecx

  result = PalTaskGetTaskData(0);
  v1 = result + 824;
  if ( !byte_57D790[result] && *((_BYTE *)&word_54CED4 + v1) )
  {
    result = *(int *)((char *)&dword_54CEC8 + v1);
    v2 = result - 1;
    if ( !result )
      v2 = 0;
    if ( v2 >= 0 )
    {
      if ( *(int *)((char *)&dword_54CECC + v1) )
      {
        sub_44B7B0(*(_DWORD *)(v1 + 804060), 0);
        result = *(int *)((char *)&dword_54CECC + v1) + *(_DWORD *)(v1 + 804064);
        *(_DWORD *)(v1 + 804060) = result;
        byte_57D458[v1] = 1;
      }
    }
  }
  return result;
}