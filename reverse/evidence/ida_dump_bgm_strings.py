"""Collect BGM wrapper and CSV-name callers in an already-copied EXE database."""
import os
from pathlib import Path
import ida_auto
import ida_funcs
import idautils
ida_auto.auto_wait()
addresses = set()
for item in idautils.Strings():
    value = str(item).lower()
    if value.startswith(('bgm_play ', 'bgm_play2 ', 'bgm_stop ', 'bgm_load ')) or 'bgm.csv' in value:
        for xref in idautils.XrefsTo(item.ea):
            function = ida_funcs.get_func(xref.frm)
            if function:
                addresses.add(function.start_ea)
os.environ['IDA_EVIDENCE_EAS'] = ','.join(hex(ea) for ea in sorted(addresses))
exec(Path('reverse/evidence/ida_dump_functions.py').read_text())
