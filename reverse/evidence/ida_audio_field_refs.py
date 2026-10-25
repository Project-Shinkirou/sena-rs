"""Collect instruction references to selected audio/video structure offsets."""
import json
import os
from pathlib import Path

import ida_auto
import ida_funcs
import ida_ua
import idautils
import idc

ida_auto.auto_wait()
offsets = {int(value, 0) for value in os.environ["IDA_FIELD_OFFSETS"].split(",")}
rows = []
for start in idautils.Functions():
    for ea in idautils.FuncItems(start):
        insn = ida_ua.insn_t()
        if not ida_ua.decode_insn(insn, ea):
            continue
        for operand in insn.ops:
            if operand.type == ida_ua.o_displ and operand.addr in offsets:
                rows.append({"ea": hex(ea), "function": hex(start),
                             "name": ida_funcs.get_func_name(start),
                             "offset": hex(operand.addr),
                             "instruction": idc.generate_disasm_line(ea, 0)})
Path(os.environ["IDA_EVIDENCE_OUT"]).write_text(json.dumps(rows, indent=2) + "\n")
idc.qexit(0)
