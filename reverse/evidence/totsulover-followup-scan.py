"""Audit new-build text/config consumers on a copied IDA database."""
import json,os
from pathlib import Path
import ida_auto,ida_funcs,ida_hexrays,idautils,idc,ida_name,ida_ua
ida_auto.auto_wait();ida_hexrays.init_hexrays_plugin()
globals_=[0x6f8fe8,0x4eb5b8,0x6f89e4]
offsets={0x55f88,0x55f8c,0x55f90,0x15f50,0x15f54,0x15f70,0x15f74}
refs=[];seeds={0x434100,0x462350}
for ea in globals_:
 for x in idautils.XrefsTo(ea):
  f=ida_funcs.get_func(x.frm)
  refs.append({'kind':'global','target':hex(ea),'from':hex(x.frm),'function':hex(f.start_ea) if f else None,'instruction':idc.generate_disasm_line(x.frm,0)})
  if f:seeds.add(f.start_ea)
for fea in idautils.Functions():
 for ea in idautils.FuncItems(fea):
  insn=ida_ua.insn_t()
  if not ida_ua.decode_insn(insn,ea):continue
  for operand in insn.ops:
   if operand.type==ida_ua.o_displ and operand.addr in offsets:
    refs.append({'kind':'offset','target':hex(operand.addr),'from':hex(ea),'function':hex(fea),'instruction':idc.generate_disasm_line(ea,0)})
    seeds.add(fea)
functions=[]
for ea in sorted(seeds):
 r={'ea':hex(ea),'name':ida_name.get_name(ea),'xrefs':[{'from':hex(x.frm),'function':ida_funcs.get_func_name(x.frm)} for x in idautils.XrefsTo(ea)]}
 try:r['pseudocode']=str(ida_hexrays.decompile(ea))
 except Exception as e:r['error']=str(e)
 r['disassembly']=[[hex(h),idc.generate_disasm_line(h,0)] for h in idautils.FuncItems(ea)]
 functions.append(r)
Path(os.environ['IDA_EVIDENCE_OUT']).write_text(json.dumps({'binary':'totsulover_try.exe','binary_sha256':'4778fe70629a7c0ee6cb3caa30ce572cc45440ac392ea998b406ca45ec304483','references':refs,'functions':functions},indent=2));idc.qexit(0)
