"""Collect image loader pointer initialization and call boundaries."""
import json,os
from pathlib import Path
import ida_auto,ida_funcs,ida_hexrays,idautils,idc,ida_name
ida_auto.auto_wait();ida_hexrays.init_hexrays_plugin()
targets=[0x4eb684,0x4eb68c,0x4eb694,0x4eb69c,0x4eb6a4,0x4eb6ac,0x4eb6c4,0x4eb6cc,0x479900]
refs=[];seeds={0x461900}
for target in targets:
 for x in idautils.XrefsTo(target):
  f=ida_funcs.get_func(x.frm)
  refs.append({'target':hex(target),'from':hex(x.frm),'function':hex(f.start_ea) if f else None,'instruction':idc.generate_disasm_line(x.frm,0)})
  if target!=0x479900 and f:seeds.add(f.start_ea)
functions=[]
for ea in sorted(seeds):
 r={'ea':hex(ea),'name':ida_name.get_name(ea)}
 try:r['pseudocode']=str(ida_hexrays.decompile(ea))
 except Exception as e:r['error']=str(e)
 r['disassembly']=[[hex(h),idc.generate_disasm_line(h,0)] for h in idautils.FuncItems(ea)];functions.append(r)
Path(os.environ['IDA_EVIDENCE_OUT']).write_text(json.dumps({'binary':'totsulover_try.exe','binary_sha256':'4778fe70629a7c0ee6cb3caa30ce572cc45440ac392ea998b406ca45ec304483','references':refs,'functions':functions},indent=2));idc.qexit(0)
