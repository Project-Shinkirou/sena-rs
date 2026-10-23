// IDA evidence from the local Game.exe at 0x42cf50.
// Opcode 23 extcall handler. Reads 2 script DWORDs: instr1=[cat<<16|idx], instr2=arg. Dispatch: gExtcallCategoryTable[cat][idx](ctx). Stores instr1->ctx[201018], instr2->ctx[184629].
int __cdecl VmOpc_Extcall(ScriptContext *ctx)
{
  ScriptContext v1; // eax
  ScriptContext v2; // ecx
  ScriptContext v3; // esi
  ScriptContext v4; // eax
  VmExtcallHandler *v5; // ecx

  v1 = ctx[201015];
  v2 = ctx[201013];
  v3 = *(_DWORD *)(v2 + v1);
  v1 += 4;
  ctx[201015] = v1;
  v4 = *(_DWORD *)(v1 + v2);
  ctx[201015] += 4;
  ctx[184629] = v4;
  ctx[201018] = v3;
  v5 = gExtcallCategoryTable[HIWORD(v3)];
  if ( v5 )
    return v5[(unsigned __int16)v3](ctx);
  else
    return 0;
}

