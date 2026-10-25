"""Dump native image suffix tables and their callers from a copied EXE."""
import json,os
from pathlib import Path
import ida_auto,ida_bytes,ida_funcs,ida_hexrays,idautils,idc
ida_auto.auto_wait();ida_hexrays.init_hexrays_plugin()
strings=[];seeds=set()
literal_strings = idautils.Strings()
literal_strings.setup(minlen=3)
for s in literal_strings:
    if str(s).lower() not in ['.pgd','.jpg','.jpeg','.tga','.bmp','.png']:continue
    refs=[]
    for x in idautils.XrefsTo(s.ea):
        f=ida_funcs.get_func(x.frm)
        if f:seeds.add(f.start_ea)
        else:
            for start in range(x.frm-20,x.frm+4,4):
                for y in idautils.XrefsTo(start):
                    f=ida_funcs.get_func(y.frm)
                    if f:seeds.add(f.start_ea)
            refs.append({'ea':hex(x.frm),'nearby':[{'ea':hex(x.frm+i*4),'value':hex(ida_bytes.get_dword(x.frm+i*4)),'string':str(idc.get_strlit_contents(ida_bytes.get_dword(x.frm+i*4)))} for i in range(-3,5)]})
    strings.append({'ea':hex(s.ea),'string':str(s),'refs':refs})
rows=[]
for ea in sorted(seeds):
    row={'ea':hex(ea),'name':ida_funcs.get_func_name(ea)}
    try:row['pseudocode']=str(ida_hexrays.decompile(ea))
    except Exception as e:row['error']=str(e)
    row['disassembly']=[[hex(h),idc.generate_disasm_line(h,0)] for h in idautils.FuncItems(ea)]
    rows.append(row)
Path(os.environ['IDA_EVIDENCE_OUT']).write_text(json.dumps({'binary':'totsulover_try.exe','sha256':'4778fe70629a7c0ee6cb3caa30ce572cc45440ac392ea998b406ca45ec304483','strings':strings,'functions':rows},indent=2))
idc.qexit(0)
