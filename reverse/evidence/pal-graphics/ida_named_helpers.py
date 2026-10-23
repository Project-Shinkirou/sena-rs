"""Read graphics helper bodies from an isolated IDA database copy."""

import json
import os
from pathlib import Path

import ida_auto
import ida_funcs
import ida_hexrays
import ida_name
import idautils
import idc


ida_auto.auto_wait()
ida_hexrays.init_hexrays_plugin()
rows = []
for name in os.environ["IDA_GRAPHICS_NAMES"].split(","):
    address = int(name, 16) if name.startswith("0x") else ida_name.get_name_ea(idc.BADADDR, name)
    function = ida_funcs.get_func(address)
    if not function:
        rows.append({"name": name, "error": "Function not found"})
        continue
    row = {
        "ea": hex(function.start_ea),
        "name": ida_funcs.get_func_name(function.start_ea),
        "disassembly": [[hex(ea), idc.generate_disasm_line(ea, 0)]
                        for ea in idautils.FuncItems(function.start_ea)],
        "xrefs": [{"from": hex(x.frm), "function": ida_funcs.get_func_name(x.frm)}
                  for x in idautils.XrefsTo(function.start_ea)],
    }
    try:
        row["pseudocode"] = str(ida_hexrays.decompile(function.start_ea))
    except Exception as exc:
        row["error"] = str(exc)
    rows.append(row)
Path(os.environ["IDA_EVIDENCE_OUT"]).write_text(json.dumps({"functions": rows}, indent=2))
idc.qexit(0)
