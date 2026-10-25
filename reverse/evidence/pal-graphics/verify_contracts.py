"""Check graphics contract coverage and references against the native inventories."""

import hashlib
import json
from collections import Counter
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]
DIRECTORY = Path(__file__).resolve().parent
contracts = json.loads((ROOT / "reverse/contracts/graphics.json").read_text())
native = json.loads((ROOT / "reverse/evidence/native-interface-inventory.json").read_text())
newer = json.loads((ROOT / "reverse/evidence/totsulover-interface-inventory.json").read_text())


def canonical(name):
    return name[1:].split("@@", 1)[0] if name.startswith("?") else name


SIBLINGS = {
    "PalGetRender", "PalAlphaCopyFilter", "PalChannelCopyEx", "PalBoxBlurFilter",
    "PalGaussianBlurFilter", "PalMotionBlurFilter", "PalGetPixel", "PalSetPixel",
    "StretchImage", "PalSetFxEffect", "PalReleaseFxEffect", "PalGetFxEffectState",
}


def selected(name):
    plain = canonical(name)
    return plain.startswith(("PalSprite", "PalRender", "PalEffect", "PalRaster",
                             "PalCopy", "PalMix", "PalTargetRenderList")) or plain in SIBLINGS


inventories = {
    "totsulover": newer["dll/Pal.dll"]["exports"],
    "reference": native["release_exports"],
    "koikake": native["local_fixture"]["pal_exports"],
}
expected = set()
for build, exports in inventories.items():
    for entry in exports:
        for name in entry["names"]:
            if selected(name):
                expected.add((build, name, entry["address"].lower()))

actual = [(row["build"], row["name"], row["address"].lower())
          for row in contracts["functions"]]
assert len(actual) == len(set(actual)), "Duplicate contract entries"
assert set(actual) == expected, {"missing": sorted(expected - set(actual)),
                                 "extra": sorted(set(actual) - expected)}

for row in contracts["functions"]:
    assert row["binary_sha256"] == contracts["builds"][row["build"]]
    assert row["behavior"] and row["portable_mapping"]
    assert row["canonical_name"] == canonical(row["name"])
    for reference in row["evidence"]:
        assert (ROOT / reference).is_file(), reference
    evidence = json.loads((ROOT / row["evidence"][0]).read_text())
    matches = [item for item in evidence["functions"]
               if item["ea"].lower() == row["address"].lower()]
    assert matches and matches[0].get("disassembly"), row["name"]
    assert evidence["sha256"] == row["binary_sha256"]
    offset = 4
    for argument in row["arguments"]:
        assert argument["stack_offset_from_entry_esp"] == offset
        offset += argument["width_bytes"]

fx_files = [ROOT / "reverse/FX.dll", ROOT / "testcase/dll/FX.dll",
            ROOT / "target/reverse-audit/totsulover/FX.dll"]
fx = [{"path": str(path.relative_to(ROOT)), "size": path.stat().st_size,
       "sha256": hashlib.sha256(path.read_bytes()).hexdigest()} for path in fx_files]
assert len({row["sha256"] for row in fx}) == 1
report = {
    "status": "passed",
    "export_instances": len(actual),
    "coverage": dict(Counter(row[0] for row in actual)),
    "abi_status_counts": dict(Counter(row["abi_status"] for row in contracts["functions"])),
    "behavior_status_counts": dict(Counter(row["behavior_status"] for row in contracts["functions"])),
    "fx_binaries": fx,
    "checks": ["Exact selected export names and addresses match all three inventories",
               "No duplicate export records", "All contract evidence files exist",
               "All export bodies have disassembly", "Build hashes match evidence",
               "Stack argument offsets respect 4/8-byte widths",
               "All three FX binaries are byte-for-byte identical by SHA-256"],
    "limitation": "This validates evidence integrity and inventory coverage, not implementation parity.",
}
(DIRECTORY / "validation.json").write_text(json.dumps(report, indent=2) + "\n")
print(json.dumps(report, indent=2))
