"""Recover explicit command registrations from the copied newer EXE."""
import json,os
from pathlib import Path
import ida_auto, ida_funcs, ida_hexrays, ida_name,idautils,idc
ida_auto.auto_wait();ida_hexrays.init_hexrays_plugin()
rows=[]
for x in idautils.XrefsTo(0x4046f0):
    h=x.frm; trace=[]
    for _ in range(6):
        h=idc.prev_head(h);trace.append([hex(h),idc.generate_disasm_line(h,0)])
    rows.append({'call':hex(x.frm),'function':ida_funcs.get_func_name(x.frm),'preceding':list(reversed(trace))})
functions=[]
for ea in [0x41cda0,0x475920,0x4764f0,0x404810]:
    if not ida_funcs.get_func(ea):ida_funcs.add_func(ea)
    r={'ea':hex(ea),'xrefs':[{'from':hex(x.frm),'function':ida_funcs.get_func_name(x.frm)} for x in idautils.XrefsTo(ea)]}
    try:r['pseudocode']=str(ida_hexrays.decompile(ea))
    except Exception as e:r['error']=str(e)
    r['disassembly']=[[hex(h),idc.generate_disasm_line(h,0)] for h in idautils.FuncItems(ea)]
    functions.append(r)
Path(os.environ['IDA_EVIDENCE_OUT']).write_text(json.dumps({'registrations':rows,'functions':functions},indent=2));idc.qexit(0)
