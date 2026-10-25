"""Validate platform contract evidence, exact export coverage and stack widths."""

import json
from collections import Counter
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]
DIRECTORY = Path(__file__).resolve().parent
contracts = json.loads((ROOT / "reverse/contracts/platform_lifecycle.json").read_text())
native = json.loads((ROOT / "reverse/evidence/native-interface-inventory.json").read_text())
newer = json.loads((ROOT / "reverse/evidence/totsulover-interface-inventory.json").read_text())
inventories = {"totsulover": newer["dll/Pal.dll"]["exports"],
               "reference": native["release_exports"],
               "koikake": native["local_fixture"]["pal_exports"]}

EXPLICIT = set("""GetMinWindowSize IsActive IsSystemEnd GetMemoryPool DebugSetActionList
PalInit PalUninit PalRun PalEnd PalDefaultCreate PalAttachProcess PalAttachWorkProcess
PalBeginUpdate PalEndUpdate PalFrameRateSet PalGetCore PalGetSystemParameters PalGetCmdLine
PalGetWindowHandle PalGetInstanceHandle PalGetlpPool Pal3DIs PalIsWorkThreadId PalGetAllocCount
PalUpdateTime PaltimeGetTime PalErrorLogPuts PalErrorLogPrintf PalErrorLogPrintfBasic
PalGetFilePath PalSaveScreenShot""".split())


def canonical(name):
    return name[1:].split("@@", 1)[0] if name.startswith("?") else name


def selected(name):
    plain = canonical(name)
    return plain in EXPLICIT or plain.startswith(("PalWait", "PalDebug")) or (
        any(part in plain for part in ("Window", "Desktop", "Aspect"))
        and plain.startswith("Pal") and not plain.startswith("PalSprite"))


expected = {(build, name, export["address"].lower())
            for build, exports in inventories.items() for export in exports
            for name in export["names"] if selected(name)}
actual = [(row["build"], row["name"], row["address"].lower())
          for row in contracts["functions"]]
assert len(actual) == len(set(actual)), "Duplicate contracts"
assert set(actual) == expected, {"missing": sorted(expected - set(actual)),
                                 "extra": sorted(set(actual) - expected)}

for row in contracts["functions"]:
    assert row["binary_sha256"] == contracts["builds"][row["build"]]
    assert row["behavior"] and row["portable_mapping"]
    assert row["canonical_name"] == canonical(row["name"])
    assert "|" not in row["behavior"], row["name"]
    for reference in row["evidence"]:
        path = ROOT / reference
        assert path.is_file(), reference
        data = json.loads(path.read_text())
        assert data["sha256"] == row["binary_sha256"]
    export_evidence = json.loads((ROOT / row["evidence"][0]).read_text())
    matching = [item for item in export_evidence["functions"] if item["ea"] == row["address"]]
    assert len(matching) == 1 and matching[0].get("disassembly"), row["name"]
    for index, argument in enumerate(row["arguments"]):
        assert argument["width_bytes"] == 4
        assert argument["stack_offset_from_entry_esp"] == 4 + index * 4
    for address in row.get("reviewed_helper_addresses", []):
        assert any(any(item.get("ea") == address and item.get("disassembly")
                       for item in json.loads((ROOT / reference).read_text())["functions"])
                   for reference in row["evidence"]), (row["name"], address)

layout = contracts["shared_layouts"][0]
assert layout["size"] == 108 and layout["alignment"] == 4
assert [item["offset"] for item in layout["fields"]] == list(range(0, 108, 4))
report = {"status": "passed", "export_instances": len(actual),
          "coverage": dict(Counter(row[0] for row in actual)),
          "abi_status_counts": dict(Counter(row["abi_status"] for row in contracts["functions"])),
          "checks": ["Exact scope matches all three native inventories",
                     "Every export record has raw disassembly and matching build hash",
                     "Every referenced reviewed helper has raw disassembly",
                     "No duplicate or malformed contract rows",
                     "All arguments preserve four-byte native stack slots",
                     "SystemParameters32 has 27 contiguous DWORD fields"],
          "limitation": "Evidence integrity and export-boundary coverage do not certify implementation parity."}
(DIRECTORY / "validation.json").write_text(json.dumps(report, indent=2) + "\n")
print(json.dumps(report, indent=2))
