import json,os
from pathlib import Path
import ida_auto,ida_funcs,ida_hexrays,idautils,idc
ida_auto.auto_wait();ida_hexrays.init_hexrays_plugin();ss=idautils.Strings();ss.setup(minlen=2)
rows=[];seeds=set()
for s in ss:
 t=str(s)
 if t.lower() in ['br','<br>','<br/>','<br />'] or ('pan' in t.lower() and ('se_' in t.lower() or 'width' in t.lower())):
  refs=[]
  for x in idautils.XrefsTo(s.ea):
   f=ida_funcs.get_func(x.frm)
   if f:seeds.add(f.start_ea);refs.append(hex(f.start_ea))
  rows.append(dict(ea=hex(s.ea),text=t,refs=refs))
funcs=[]
for ea in sorted(seeds):
 f=dict(ea=hex(ea),name=ida_funcs.get_func_name(ea),disassembly=[[hex(h),idc.generate_disasm_line(h,0)] for h in idautils.FuncItems(ea)])
 try:f['pseudocode']=str(ida_hexrays.decompile(ea))
 except Exception as e:f['error']=str(e)
 funcs.append(f)
Path(os.environ['IDA_EVIDENCE_OUT']).write_text(json.dumps(dict(strings=rows,functions=funcs),indent=2));idc.qexit(0)
