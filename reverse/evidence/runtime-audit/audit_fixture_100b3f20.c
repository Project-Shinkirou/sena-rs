// Fresh IDA evidence from testcase/dll/Pal.dll, analyzed from a copy.
int __cdecl sub_100B3F20(_DWORD *a1, int a2)
{
  a1[3] = 2;
  a1[2] = a2;
  a1[1] = PaltimeGetTime();
  *a1 = 1;
  return 1;
}

