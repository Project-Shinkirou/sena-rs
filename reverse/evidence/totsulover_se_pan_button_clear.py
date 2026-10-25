import json,os
from pathlib import Path
import ida_auto,ida_funcs,ida_hexrays,idautils,idc
ida_auto.auto_wait();ida_hexrays.init_hexrays_plugin();rows=[]
for ea in idautils.Functions():
 ins=[[hex(h),idc.generate_disasm_line(h,0)] for h in idautils.FuncItems(ea)]
 if ea==0x422680 or any('26FB0h' in (x[1] or '') for x in ins):
  f=dict(ea=hex(ea),name=ida_funcs.get_func_name(ea),disassembly=ins)
  try:f['pseudocode']=str(ida_hexrays.decompile(ea))
  except Exception as e:f['error']=str(e)
  rows.append(f)
Path(os.environ['IDA_EVIDENCE_OUT']).write_text(json.dumps(dict(binary='totsulover_try.exe',sha256='4778fe70629a7c0ee6cb3caa30ce572cc45440ac392ea998b406ca45ec304483',functions=rows),indent=2));idc.qexit(0)
