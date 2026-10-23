"""Inventory local PE interfaces and FX resources without modifying game files.

Run from the repository root. This records native surface area, not a count of
implemented Rust handlers. PAL.sqlite is the debug DLL's IDA export.
"""
import hashlib
import json
import sqlite3
import struct
from pathlib import Path


class PE:
    def __init__(self, path):
        self.path = Path(path)
        self.data = self.path.read_bytes()
        pe = self.u32(0x3C)
        assert self.data[pe:pe + 4] == b"PE\0\0"
        self.optional = pe + 24
        assert self.u16(self.optional) == 0x10B, "Expected PE32"
        self.base = self.u32(self.optional + 28)
        section = self.optional + self.u16(pe + 20)
        self.sections = []
        for i in range(self.u16(pe + 6)):
            off = section + 40 * i
            size, rva, raw_size, raw = struct.unpack_from("<4I", self.data, off + 8)
            self.sections.append((rva, max(size, raw_size), raw))

    def u16(self, off):
        return struct.unpack_from("<H", self.data, off)[0]

    def u32(self, off):
        return struct.unpack_from("<I", self.data, off)[0]

    def offset(self, rva):
        for start, size, raw in self.sections:
            if start <= rva < start + size:
                return raw + rva - start
        raise ValueError(f"Unmapped RVA: {rva:#x}")

    def string(self, rva):
        return self.data[self.offset(rva):].split(b"\0", 1)[0].decode("ascii")

    def directory(self, index):
        return self.u32(self.optional + 96 + 8 * index)

    def exports(self):
        if not self.directory(0):
            return []
        off = self.offset(self.directory(0))
        base, count, names, functions, name_ptrs, ordinals = struct.unpack_from(
            "<6I", self.data, off + 16)
        labels = {}
        for i in range(names):
            index = self.u16(self.offset(ordinals) + 2 * i)
            labels.setdefault(index, []).append(self.string(
                self.u32(self.offset(name_ptrs) + 4 * i)))
        return [dict(ordinal=base + i, address=hex(self.base + address),
                     names=labels.get(i, []))
                for i in range(count)
                if (address := self.u32(self.offset(functions) + 4 * i))]

    def imports(self):
        result = []
        if not self.directory(1):
            return result
        off = self.offset(self.directory(1))
        while self.u32(off + 12):
            module = self.string(self.u32(off + 12))
            lookup = self.offset(self.u32(off) or self.u32(off + 16))
            iat = self.u32(off + 16)
            index = 0
            while (value := self.u32(lookup + 4 * index)):
                result.append(dict(module=module, binding="normal", iat=hex(self.base + iat + index * 4),
                                   name=self.string(value + 2) if not value & 0x80000000 else None,
                                   ordinal=(value & 0xFFFF) if value & 0x80000000 else None))
                index += 1
            off += 20
        delay_rva = self.directory(13)
        if delay_rva:
            off = self.offset(delay_rva)
            while self.u32(off + 4):
                attrs, name, _, iat, lookup, _, _, _ = struct.unpack_from("<8I", self.data, off)
                bias = 0 if attrs & 1 else self.base
                module = self.string(name - bias)
                lookup = self.offset(lookup - bias)
                index = 0
                while (value := self.u32(lookup + 4 * index)):
                    result.append(dict(module=module, binding="delay", iat=hex(self.base + iat - bias + index * 4),
                                       name=self.string(value - bias + 2) if not value & 0x80000000 else None,
                                       ordinal=(value & 0xFFFF) if value & 0x80000000 else None))
                    index += 1
                off += 32
        return result

    def resources(self):
        if not self.directory(2):
            return []
        root = self.offset(self.directory(2))
        result = []

        def walk(relative, names):
            directory = root + relative
            count = self.u16(directory + 12) + self.u16(directory + 14)
            for i in range(count):
                label, target = struct.unpack_from("<II", self.data, directory + 16 + 8 * i)
                if label & 0x80000000:
                    p = root + (label & 0x7FFFFFFF)
                    label = self.data[p + 2:p + 2 + 2 * self.u16(p)].decode("utf-16le")
                path = names + [label]
                if target & 0x80000000:
                    walk(target & 0x7FFFFFFF, path)
                else:
                    rva, size = struct.unpack_from("<II", self.data, root + target)
                    raw = self.offset(rva)
                    blob = self.data[raw:raw + size]
                    row = dict(path=path, rva=hex(rva), file_offset=hex(raw), size=size,
                               sha256=hashlib.sha256(blob).hexdigest())
                    if "RASTER_SW" in path:
                        row["hlsl"] = blob.rstrip(b"\0").decode("ascii")
                    result.append(row)

        walk(0, [])
        return result


def main():
    game, release, debug, fx = [PE("reverse/" + name) for name in
                               ["Game.exe", "PAL.dll", "PAL_d.dll", "FX.dll"]]
    imports = [r for r in game.imports() if r["module"].lower() in ("pal", "pal.dll")]
    release_exports, debug_exports = release.exports(), debug.exports()
    release_names = {n for r in release_exports for n in r["names"]}
    debug_names = {n for r in debug_exports for n in r["names"]}
    inventory = dict(
        scope="Local binaries only; interface presence does not certify implementation parity",
        binaries={p.path.name: dict(size=len(p.data), sha256=hashlib.sha256(p.data).hexdigest())
                  for p in [game, release, debug, fx]},
        game_pal_imports=imports, release_exports=release_exports, debug_exports=debug_exports,
        release_only_names=sorted(release_names - debug_names),
        debug_only_names=sorted(debug_names - release_names),
        game_imports_missing_from_debug=sorted({r["name"] for r in imports} - debug_names),
        ida_database_identity={})
    for path, pe in [("reverse/Game.sqlite", game), ("reverse/PAL.sqlite", debug)]:
        with sqlite3.connect("file:" + path + "?mode=ro", uri=True) as db:
            metadata = dict(db.execute("select k,v from meta"))
        inventory["ida_database_identity"][path] = dict(
            root_filename=metadata["root_filename"],
            input_md5_matches=metadata["input_file_md5"] == hashlib.md5(pe.data).hexdigest())
    fixture_game = Path("testcase/koikake.exe")
    fixture_pal = Path("testcase/dll/Pal.dll")
    if fixture_game.exists() and fixture_pal.exists():
        fg, fp = PE(fixture_game), PE(fixture_pal)
        fixture_imports = [r for r in fg.imports()
                           if r["module"].lower() in ("pal", "pal.dll")]
        inventory["local_fixture"] = dict(
            binaries={p.path.as_posix(): dict(size=len(p.data),
                       sha256=hashlib.sha256(p.data).hexdigest()) for p in [fg, fp]},
            game_pal_imports=fixture_imports, pal_exports=fp.exports(),
            same_game_as_reverse=fg.data == game.data,
            same_pal_as_reverse=fp.data == release.data)
        fixture_fx = Path("testcase/dll/FX.dll")
        if fixture_fx.exists():
            inventory["local_fixture"]["same_fx_as_reverse"] = fixture_fx.read_bytes() == fx.data
    inventory["audited_source_sha256"] = {
        path: hashlib.sha256(Path(path).read_bytes()).hexdigest()
        for path in ["crates/pal-vm/src/runtime.rs", "crates/pal-vm/src/sprite.rs",
                     "crates/pal-vm/src/effect.rs", "crates/pal-vm/src/scene.rs",
                     "crates/pal-vm/src/renderer/shader.rs",
                     "crates/pal-vm/src/renderer/wgpu_backend.rs",
                     "crates/pal-vm/src/fx.rs", "crates/pal-vm/src/ffi/scene.rs",
                     "crates/pal-vm/src/runtime/primary_aliases.rs",
                     "crates/pal-vm/src/renderer/shaders/raster.wgsl",
                     "platform/switch/source/main.c", "platform/switch/source/scene_gpu.c",
                     "platform/switch/source/gpu.c", "platform/vita/source/main.c",
                     "platform/vita/source/scene_gpu.c"]}
    out = Path("reverse/evidence")
    (out / "native-interface-inventory.json").write_text(json.dumps(inventory, indent=2) + "\n")
    resources = fx.resources()
    (out / "fx-resources.json").write_text(json.dumps(resources, indent=2) + "\n")
    print(json.dumps(dict(game_pal_imports=len(imports), release_exports=len(release_exports),
                          debug_exports=len(debug_exports),
                          imports_missing_from_debug=inventory["game_imports_missing_from_debug"],
                          fx_resources=[r["path"] for r in resources]), indent=2))


if __name__ == "__main__":
    main()
