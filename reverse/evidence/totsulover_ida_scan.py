"""Collect dispatch-registration and imported-call evidence from a copied EXE."""
import json, os
from pathlib import Path
import ida_auto, ida_funcs, ida_hexrays, ida_name, ida_nalt, idautils, idc
ida_auto.auto_wait()
ida_hexrays.init_hexrays_plugin()
rows=[]
seeds=set()
strings=[]
for s in idautils.Strings():
    t=str(s)
    if any(p in t for p in ['error command table','error return command','Script.src','stack point:', 'sound_set_freq', 'voice_speed']):
        xrefs=[{'from':hex(x.frm),'function':ida_funcs.get_func_name(x.frm)} for x in idautils.XrefsTo(s.ea)]
        strings.append({'ea':hex(s.ea),'text':t,'xrefs':xrefs})
        for x in idautils.XrefsTo(s.ea):
            f=ida_funcs.get_func(x.frm)
            if f:seeds.add(f.start_ea)
imports=[]
for i in range(ida_nalt.get_import_module_qty()):
    if 'pal' not in ida_nalt.get_import_module_name(i).lower():continue
    def add(ea,name,ordinal):
        xr=[{'from':hex(x.frm),'function':ida_funcs.get_func_name(x.frm)} for x in idautils.XrefsTo(ea)]
        imports.append({'ea':hex(ea),'name':name,'xrefs':xr})
        if any(p in (name or '').lower() for p in ['freq','speed','desktop','monitor','filter']):
            for x in idautils.XrefsTo(ea):
                f=ida_funcs.get_func(x.frm)
                if f:seeds.add(f.start_ea)
        return True
    ida_nalt.enum_import_names(i,add)
for ea in sorted(seeds):
    row={'ea':hex(ea),'name':ida_name.get_name(ea),'xrefs':[{'from':hex(x.frm),'function':ida_funcs.get_func_name(x.frm)} for x in idautils.XrefsTo(ea)]}
    try:row['pseudocode']=str(ida_hexrays.decompile(ea))
    except Exception as e:row['error']=str(e)
    row['disassembly']=[[hex(h),idc.generate_disasm_line(h,0)] for h in idautils.FuncItems(ea)]
    rows.append(row)
Path(os.environ['IDA_EVIDENCE_OUT']).write_text(json.dumps({'strings':strings,'pal_imports':imports,'functions':rows},indent=2))
idc.qexit(0)
