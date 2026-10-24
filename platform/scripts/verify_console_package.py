#!/usr/bin/env python3
"""Reject incomplete console packages before uploading CI artifacts."""
import argparse
import hashlib
import json
import struct
import zipfile
from pathlib import Path


def verify(platform, path):
    if platform == 'switch':
        with path.open('rb') as source:
            header = source.read(0x80)
        if len(header) != 0x80 or header[0x10:0x14] != b'NRO0':
            raise ValueError('missing Switch NRO header')
        size = struct.unpack_from('<I', header, 0x18)[0]
        if not 0x80 <= size <= path.stat().st_size:
            raise ValueError('invalid or truncated NRO image size')
    else:
        with zipfile.ZipFile(path) as package:
            for name, magic in [('eboot.bin', b'SCE\0'),
                                ('sce_sys/param.sfo', b'\0PSF')]:
                if package.namelist().count(name) != 1:
                    raise ValueError(f'expected exactly one {name}')
                with package.open(name) as source:
                    if source.read(4) != magic:
                        raise ValueError(f'invalid {name} header')
            bad_entry = package.testzip()
            if bad_entry:
                raise ValueError(f'VPK CRC failure: {bad_entry}')
    digest = hashlib.sha256()
    with path.open('rb') as source:
        for chunk in iter(lambda: source.read(1024 * 1024), b''):
            digest.update(chunk)
    return {'platform': platform, 'path': str(path),
            'bytes': path.stat().st_size, 'sha256': digest.hexdigest()}


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('platform', choices=['switch', 'vita'])
    parser.add_argument('package', type=Path)
    args = parser.parse_args()
    try:
        result = verify(args.platform, args.package)
    except (OSError, ValueError, zipfile.BadZipFile) as error:
        parser.exit(1, f'Invalid console package: {error}\n')
    print(json.dumps(result, indent=2))


if __name__ == '__main__':
    main()
