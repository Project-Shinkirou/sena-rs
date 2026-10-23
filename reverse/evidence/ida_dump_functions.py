"""Batch IDAPython evidence dump, run against a COPY of an existing database.

IDA_EVIDENCE_OUT names the JSON output. IDA_EVIDENCE_EAS is a comma-separated
list of addresses or exact IDA names. No renames, comments, or types are applied to the database.
"""

import json
import os
from pathlib import Path

import ida_auto
import ida_bytes
import ida_funcs
import ida_hexrays
import ida_name
import ida_nalt
import idautils
import idc


def main():
    output = Path(os.environ["IDA_EVIDENCE_OUT"])
    try:
        ida_auto.auto_wait()
        ida_hexrays.init_hexrays_plugin()
        functions = []
        for address in os.environ.get("IDA_EVIDENCE_EAS", "").split(","):
            if not address.strip():
                continue
            try:
                ea = int(address, 0)
            except ValueError:
                ea = ida_name.get_name_ea(idc.BADADDR, address.strip())
                if ea == idc.BADADDR:
                    raise ValueError(f"IDA name not found: {address}")
            function = ida_funcs.get_func(ea)
            row = {"ea": hex(ea), "name": ida_name.get_name(ea)}
            row["xrefs"] = [
                {"from": hex(x.frm), "function": ida_funcs.get_func_name(x.frm)}
                for x in idautils.XrefsTo(ea)
            ]
            if function:
                try:
                    row["pseudocode"] = str(ida_hexrays.decompile(function.start_ea))
                except Exception as exc:
                    row["error"] = str(exc)
                row["disassembly"] = [
                    [hex(head), idc.generate_disasm_line(head, 0)]
                    for head in idautils.FuncItems(function.start_ea)
                ]
            functions.append(row)
        tables = {}
        for spec in os.environ.get("IDA_EVIDENCE_TABLES", "").split(","):
            if not spec.strip():
                continue
            start, count = (int(part, 0) for part in spec.split(":"))
            tables[hex(start)] = [
                {"ea": hex(ida_bytes.get_dword(start + i * 4)),
                 "name": ida_name.get_name(ida_bytes.get_dword(start + i * 4))}
                for i in range(count)
            ]
        output.parent.mkdir(parents=True, exist_ok=True)
        output.write_text(json.dumps({"input": ida_nalt.get_input_file_path(),
                                      "functions": functions, "tables": tables},
                                     ensure_ascii=False, indent=2))
        idc.qexit(0)
    except Exception as exc:
        output.with_suffix(".error").write_text(repr(exc))
        idc.qexit(1)


main()
