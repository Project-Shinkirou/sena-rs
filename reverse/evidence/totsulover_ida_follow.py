"""Collect explicitly requested local functions and imported callers."""
import os,json
from pathlib import Path
import ida_auto,ida_funcs,ida_hexrays,idautils,idc,ida_name
ida_auto.auto_wait();ida_hexrays.init_hexrays_plugin()
seeds=[int(s,0) for s in os.environ['IDA_EVIDENCE_EAS'].split(',')]
for ea in seeds:
    if not ida_funcs.get_func(ea):ida_funcs.add_func(ea)
rows=[]
for ea in seeds:
    r={'ea':hex(ea),'name':ida_name.get_name(ea),'xrefs':[{'from':hex(x.frm),'function':ida_funcs.get_func_name(x.frm)} for x in idautils.XrefsTo(ea)]}
    try:r['pseudocode']=str(ida_hexrays.decompile(ea))
    except Exception as e:r['error']=str(e)
    r['disassembly']=[[hex(h),idc.generate_disasm_line(h,0)] for h in idautils.FuncItems(ea)]
    rows.append(r)
Path(os.environ['IDA_EVIDENCE_OUT']).write_text(json.dumps({'functions':rows},indent=2));idc.qexit(0)
