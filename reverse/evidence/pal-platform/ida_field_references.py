"""Read selected instructions, static words and field references from a copied DB."""

import json
import os
import re
from pathlib import Path

import ida_auto
import ida_bytes
import ida_funcs
import idautils
import idc

ida_auto.auto_wait()
offset = os.environ.get("IDA_FIELD_OFFSET", "368")
pattern = re.compile(r"\[[a-z0-9]+\+" + offset + r"h\]", re.I)
rows = []
for function in idautils.Functions():
    matches = []
    for address in idautils.FuncItems(function):
        instruction = idc.generate_disasm_line(address, 0) or ""
        if pattern.search(instruction):
            matches.append([hex(address), instruction])
    if matches:
        rows.append({"ea": hex(function), "name": ida_funcs.get_func_name(function),
                     "references": matches})

ranges = []
for item in os.environ.get("IDA_RAW_RANGES", "").split(","):
    if not item:
        continue
    start, end = (int(word, 0) for word in item.split(":"))
    ranges.append({"start": hex(start), "end": hex(end),
                   "disassembly": [[hex(address), idc.generate_disasm_line(address, 0)]
                                   for address in idautils.Heads(start, end)]})

words = {}
for item in os.environ.get("IDA_STATIC_WORDS", "").split(","):
    if not item:
        continue
    start, count = (int(word, 0) for word in item.split(":"))
    words[hex(start)] = [ida_bytes.get_dword(start + index * 4) for index in range(count)]

Path(os.environ["IDA_EVIDENCE_OUT"]).write_text(json.dumps({
    "field_offset": "0x" + offset, "functions": rows, "raw_ranges": ranges,
    "static_words": words,
}, indent=2) + "\n")
idc.qexit(0)
