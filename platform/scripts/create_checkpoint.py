#!/usr/bin/env python3
"""Create a host diagnostic checkpoint by replaying normal input in an isolated root."""
import argparse
import ctypes as c
import hashlib
import json
from pathlib import Path


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--library', type=Path, required=True)
    parser.add_argument('--game-root', type=Path, required=True,
                        help='Writable isolated resource root; never use an original game directory')
    parser.add_argument('--replay', type=Path, required=True)
    parser.add_argument('--after-frame', type=int, required=True)
    parser.add_argument('--slot', type=int, required=True)
    parser.add_argument('--nls', default='shift-jis')
    parser.add_argument('--lang', default='jp')
    args = parser.parse_args()
    if args.after_frame < 0 or not 0 <= args.slot < 1000:
        parser.error('frame must be nonnegative and slot must be in 0..999')
    lines = args.replay.read_text().splitlines()
    dt = int(lines[0].split(',')[0])
    if not 1 <= dt <= 100:
        parser.error('checkpoint replay requires a fixed timestep in 1..100 ms')
    rows = {}
    for line in lines[1:]:
        frame, x, y, left, skip = map(int, line.split(','))
        rows[frame] = (x, y, left, skip)
    lib = c.CDLL(str(args.library.resolve()))
    ptr, integer, unsigned = c.c_void_p, c.c_int32, c.c_uint32

    def bind(name, arguments, result=None):
        function = getattr(lib, name)
        function.argtypes, function.restype = arguments, result
        return function

    create = bind('sena_host_create', [c.c_char_p] * 3 + [unsigned] * 2, ptr)
    scene = bind('sena_host_set_scene_rendering', [ptr, integer])
    step = bind('sena_host_step', [ptr, unsigned], integer)
    width = bind('sena_host_frame_width', [ptr], unsigned)
    height = bind('sena_host_frame_height', [ptr], unsigned)
    resize = bind('sena_host_resize', [ptr, unsigned, unsigned])
    touch = bind('sena_host_touch', [ptr, integer, c.c_double, c.c_double])
    mouse = bind('sena_host_mouse_button', [ptr, unsigned, integer])
    key = bind('sena_host_key', [ptr, c.c_char_p, integer])
    save = bind('sena_host_save_checkpoint', [ptr, integer], integer)
    load = bind('sena_host_load_checkpoint', [ptr, integer], integer)
    sink_get = bind('sena_host_audio_sink', [ptr], ptr)
    render = bind('sena_audio_render_i16', [ptr, c.POINTER(c.c_int16), c.c_size_t])
    sink_destroy = bind('sena_audio_sink_destroy', [ptr])
    destroy = bind('sena_host_destroy', [ptr])

    def new_host():
        host = create(str(args.game_root.resolve()).encode(), args.nls.encode(),
                      args.lang.encode(), 960, 544)
        if not host:
            raise RuntimeError('host creation failed')
        scene(host, 1)
        return host

    host, sink = new_host(), None
    frames = 48000 * dt // 1000
    samples = (c.c_int16 * (frames * 2))()
    try:
        sink = sink_get(host)
        x, y, old_left, old_skip = width(host) / 2, height(host) / 2, 0, 0
        for frame in range(args.after_frame + 1):
            if frame in rows:
                x, y, left, skip = rows[frame]
                if left != old_left:
                    mouse(host, 0, left)
                if skip != old_skip:
                    key(host, b'Control', skip)
                old_left, old_skip = left, skip
            resize(host, width(host), height(host))
            touch(host, 1, x, y)
            if step(host, dt) != 0:
                raise RuntimeError(f'engine stopped at frame {frame}')
            render(sink, samples, frames)
        if save(host, args.slot) != 1:
            raise RuntimeError('checkpoint capture failed')
    finally:
        if sink:
            sink_destroy(sink)
        destroy(host)
    host = new_host()
    try:
        if load(host, args.slot) != 1 or step(host, dt) != 0:
            raise RuntimeError('fresh-host checkpoint validation failed')
    finally:
        destroy(host)
    path = args.game_root / 'save' / f'save{args.slot:03d}.dat'
    data = path.read_bytes()
    print(json.dumps({'path': str(path.resolve()), 'slot': args.slot,
                      'after_frame': args.after_frame, 'bytes': len(data),
                      'sha256': hashlib.sha256(data).hexdigest(),
                      'fresh_host_load': True}, indent=2))


if __name__ == '__main__':
    main()
