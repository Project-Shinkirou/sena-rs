"""Dump import callers from a copied IDA database without changing types."""
import os, json
from pathlib import Path
import ida_auto, ida_funcs, ida_hexrays, idautils, ida_name, idc
ida_auto.auto_wait()
ida_hexrays.init_hexrays_plugin()
imports=[]
starts=set()
for ea,name in idautils.Names():
    if not name.startswith(os.environ.get('IDA_IMPORT_PREFIX', '__imp_PalButton')):continue
    callers=[]
    for x in idautils.XrefsTo(ea):
        f=ida_funcs.get_func(x.frm)
        if f and not ida_funcs.get_func_name(f.start_ea).startswith(os.environ.get('IDA_IMPORT_PREFIX', '__imp_PalButton').removeprefix('__imp_')):
            starts.add(f.start_ea);callers.append(hex(f.start_ea))
    imports.append({'name':name,'ea':hex(ea),'callers':sorted(set(callers))})
functions=[]
for ea in sorted(starts):
    functions.append({'ea':hex(ea),'name':ida_name.get_name(ea),
      'pseudocode':str(ida_hexrays.decompile(ea)),
      'xrefs':[{'from':hex(x.frm),'function':ida_funcs.get_func_name(x.frm)} for x in idautils.XrefsTo(ea)],
      'disassembly':[[hex(h),idc.generate_disasm_line(h,0)] for h in idautils.FuncItems(ea)]})
Path(os.environ['IDA_EVIDENCE_OUT']).write_text(json.dumps({'functions':functions,'imports':imports},indent=2))
idc.qexit(0)
