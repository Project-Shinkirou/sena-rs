"""Find users of the reviewed SE-prefix registry in a copied IDA database."""
import json, os
from pathlib import Path
import ida_auto, ida_funcs, ida_hexrays, ida_name, idautils, idc
ida_auto.auto_wait()
ida_hexrays.init_hexrays_plugin()
seeds=set()
for ea in idautils.Functions():
    for head in idautils.FuncItems(ea):
        for operand in range(3):
            if idc.get_operand_type(head,operand) in (idc.o_displ,idc.o_imm):
                value=idc.get_operand_value(head,operand)
                if 159668 <= value < 159844:
                    seeds.add(ea)
rows=[]
for ea in sorted(seeds):
    row={"ea":hex(ea),"name":ida_name.get_name(ea)}
    try:row["pseudocode"]=str(ida_hexrays.decompile(ea))
    except Exception as error:row["error"]=str(error)
    row["disassembly"]=[[hex(h),idc.generate_disasm_line(h,0)] for h in idautils.FuncItems(ea)]
    rows.append(row)
Path(os.environ["IDA_EVIDENCE_OUT"]).write_text(json.dumps({"binary":"totsulover_try.exe","sha256":"4778fe70629a7c0ee6cb3caa30ce572cc45440ac392ea998b406ca45ec304483","functions":rows},indent=2))
idc.qexit(0)
