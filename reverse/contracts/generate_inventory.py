"""Reconcile reviewed behavior contracts with every release PAL export.

Coverage is per exact exported name and binary build. An export address or an
IDA decompilation alone never counts as a reviewed behavior or a C declaration.
"""

import argparse
import json
from collections import Counter
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
HERE = Path(__file__).resolve().parent


def read(path):
    return json.loads(path.read_text())


def exports():
    old = read(ROOT / "reverse/evidence/native-interface-inventory.json")
    new = read(ROOT / "reverse/evidence/totsulover-interface-inventory.json")
    return {
        "koikake": (old["local_fixture"]["binaries"]["testcase/dll/Pal.dll"]["sha256"],
                    old["local_fixture"]["pal_exports"]),
        "reference": (old["binaries"]["PAL.dll"]["sha256"], old["release_exports"]),
        "totsulover": (new["dll/Pal.dll"]["sha256"], new["dll/Pal.dll"]["exports"]),
    }


def generate():
    rows = {}
    for build, (digest, entries) in exports().items():
        for entry in entries:
            for name in entry["names"]:
                key = (build, name)
                assert key not in rows, key
                rows[key] = {
                    "build": build, "name": name, "ordinal": entry["ordinal"],
                    "address": entry["address"].lower(), "binary_sha256": digest,
                    "abi_status": "not_reviewed", "behavior_status": "not_reviewed",
                    "contracts": [], "declaration_manifest": None,
                    "implementation_parity": "not_certified",
                }
    for path in sorted(HERE.glob("*.json")):
        if path.name == "inventory.json":
            continue
        document = read(path)
        for function in document.get("functions", []):
            instances = function.get("builds")
            if instances is None:
                instances = [function]
            assert isinstance(instances, list), path
            for instance in instances:
                build = instance["build"]
                name = instance.get("export_name", instance.get("name", function.get("export_name", function["name"])))
                key = (build, name)
                assert key in rows, (path, key)
                row = rows[key]
                address = instance["address"].lower()
                digest = instance.get("binary_sha256", instance.get("sha256"))
                assert row["address"] == address, (path, key, address)
                assert row["binary_sha256"] == digest, (path, key, digest)
                evidence = instance.get("evidence", function.get("evidence", []))
                assert evidence, (path, key, "no evidence")
                body_found = False
                for item in evidence:
                    evidence_path = item if isinstance(item, str) else item["file"]
                    data = read(ROOT / evidence_path)
                    evidence_digest = data.get("sha256", data.get("binary_sha256"))
                    if evidence_digest is None and isinstance(data.get("profiles"), list):
                        profile = next((p for p in data["profiles"] if p["build"] == build), {})
                        evidence_digest = profile.get("sha256")
                    assert isinstance(evidence_digest, str) and len(evidence_digest) == 64, (evidence_path, "missing binary hash")
                    body_found |= evidence_digest == digest and any(
                        int(f.get("ea", f.get("address", "0")), 0) == int(address, 0)
                        and bool(f.get("disassembly"))
                        for f in data.get("functions", [])
                    )
                assert body_found, (path, key, "export body absent")
                assert not row["contracts"], (path, key, "duplicate contract owner")
                row["contracts"].append(str(path.relative_to(ROOT)))
                for status in ("abi_status", "behavior_status"):
                    row[status] = instance.get(status, function.get(status, "not_reviewed"))
                row["unresolved"] = function.get("unresolved", [])
                row["downstream_behavior_status"] = function.get("downstream_behavior_status", "see_contract")
    manifest = read(ROOT / "reverse/abi/pal32.json")
    manifests = [("reverse/abi/pal32.json", manifest)]
    manifests.extend(("reverse/abi/" + p, read(ROOT / "reverse/abi" / p))
                     for p in manifest.get("includes", []))
    for source, declarations in manifests:
        for function in declarations["functions"]:
            for build in function["evidence"]:
                key = (build, function.get("export_name", function["name"]))
                if key in rows:
                    rows[key]["declaration_manifest"] = source
    ordered = [rows[key] for key in sorted(rows)]
    summary = {}
    for build in exports():
        subset = [r for r in ordered if r["build"] == build]
        summary[build] = {
            "export_names": len(subset),
            "behavior_status": dict(sorted(Counter(r["behavior_status"] for r in subset).items())),
            "abi_status": dict(sorted(Counter(r["abi_status"] for r in subset).items())),
            "generated_declarations": sum(r["declaration_manifest"] is not None for r in subset),
        }
    result = {
        "schema_version": 1,
        "scope": "Every exact named export of the three inventoried release PAL DLLs.",
        "limitations": [
            "Export-boundary review does not establish full downstream helper or OS behavior.",
            "ABI review, generated declarations and portable implementation parity are separate claims.",
            "The VM command table, private functions and shader programs have separate inventories.",
        ],
        "summary": summary,
        "exports": ordered,
    }
    return result


def markdown(result):
    lines = ["# PAL behavior inventory", "",
             "Generated by `generate_inventory.py`. Counts are exact exported names per binary,",
             "not percentages of engine compatibility. Review covers the export boundary;",
             "private helpers, source-type ambiguity and portable differences remain explicit",
             "in each domain contract.", "",
             "| Build | Export names | Export boundaries reviewed | C/Rust declarations |",
             "|---|---:|---:|---:|"]
    for build, summary in result["summary"].items():
        total = summary["export_names"]
        reviewed = total - summary["behavior_status"].get("not_reviewed", 0)
        lines.append(f'| {build} | {total} | {reviewed} | {summary["generated_declarations"]} |')
    lines += ["", "## Domain contracts", "",
              "- [Core: list, random, memory, files, paths and tasks](core.json)",
              "- [Graphics, render lists, filters and effects](graphics.json)",
              "- [Audio, movies and their VM wrappers](audio_video.json)",
              "- [Fonts, text, numbers, thumbnails and animation](text_animation.json)",
              "- [Input, buttons, cursor, touch and gesture](input_buttons.json)",
              "- [Platform, lifecycle, window, waits and diagnostics](platform_lifecycle.json)",
              "", "## Exact export matrix", "",
              "`R` means the export boundary has a reviewed behavior record. `A?` means",
              "its source ABI still has unresolved facts. `D` means a generated C/Rust",
              "declaration exists. `Pending` means no reviewed record; `-` means the",
              "name is absent from that build. Read the linked JSON for full semantics,",
              "evidence and downstream limitations.", "",
              "| Exact exported name | Koikake | Reference | Totsulover |",
              "|---|---|---|---|"]
    rows = {(r["build"], r["name"]): r for r in result["exports"]}
    for name in sorted({r["name"] for r in result["exports"]}):
        cells = []
        for build in ("koikake", "reference", "totsulover"):
            row = rows.get((build, name))
            if row is None:
                cells.append("-")
            elif row["behavior_status"] == "not_reviewed":
                cells.append("Pending")
            else:
                status = "R"
                if row["abi_status"] not in ("confirmed", "reviewed_machine_contract"):
                    status += " A?"
                if row["declaration_manifest"]:
                    status += " D"
                target = Path(row["contracts"][0]).name
                cells.append(f"[{status}]({target})")
        lines.append(f'| `{name}` | ' + " | ".join(cells) + " |")
    return "\n".join(lines) + "\n"


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--check", action="store_true")
    parser.add_argument("--require-all-reviewed", action="store_true")
    args = parser.parse_args()
    result = generate()
    text = json.dumps(result, indent=2, ensure_ascii=True) + "\n"
    for destination, content in [(HERE / "inventory.json", text),
                                 (HERE / "README.md", markdown(result))]:
        if args.check:
            assert destination.read_text() == content, f"Stale PAL behavior inventory: {destination.name}"
        else:
            destination.write_text(content)
    print(json.dumps(result["summary"], indent=2))
    if args.require_all_reviewed:
        missing = [r for r in result["exports"] if r["behavior_status"] == "not_reviewed"]
        assert not missing, f"{len(missing)} export/build contracts remain unreviewed"


if __name__ == "__main__":
    main()
