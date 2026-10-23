"""Recover audio call-register arguments and image-loader consumers."""
import json,os
from pathlib import Path
import ida_auto,ida_funcs,ida_hexrays,idautils,idc,ida_name
ida_auto.auto_wait();ida_hexrays.init_hexrays_plugin()
seeds={0x408ce0,0x4622a0,0x4621d0,0x462100,0x462040,0x461ec0,0x462760,0x4631c0,0x4632e0,0x463360,0x4630a0}
refs=[]
for target in [0x408ce0,0x4eb680,0x4d2534]:
 for x in idautils.XrefsTo(target):
  f=ida_funcs.get_func(x.frm);h=x.frm;trace=[]
  for _ in range(14):
   h=idc.prev_head(h);trace.append([hex(h),idc.generate_disasm_line(h,0)])
  refs.append({'target':hex(target),'from':hex(x.frm),'function':hex(f.start_ea) if f else None,'preceding':list(reversed(trace))})
  if target==0x4eb680 and f:seeds.add(f.start_ea)
functions=[]
for ea in sorted(seeds):
 r={'ea':hex(ea),'name':ida_name.get_name(ea),'xrefs':[{'from':hex(x.frm),'function':ida_funcs.get_func_name(x.frm)} for x in idautils.XrefsTo(ea)]}
 try:r['pseudocode']=str(ida_hexrays.decompile(ea))
 except Exception as e:r['error']=str(e)
 r['disassembly']=[[hex(h),idc.generate_disasm_line(h,0)] for h in idautils.FuncItems(ea)]
 functions.append(r)
Path(os.environ['IDA_EVIDENCE_OUT']).write_text(json.dumps({'binary':'totsulover_try.exe','binary_sha256':'4778fe70629a7c0ee6cb3caa30ce572cc45440ac392ea998b406ca45ec304483','references':refs,'functions':functions},indent=2));idc.qexit(0)
