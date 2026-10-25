"""Decode PAL's D3D9 shader instruction tokens (no compiler or Wine required).

Token layout: https://learn.microsoft.com/windows-hardware/drivers/display/shader-code-format
The small opcode set is deliberately strict; unknown instructions fail closed.
"""
import json
import struct
from pathlib import Path

OPS = {0:'nop',1:'mov',2:'add',3:'sub',4:'mad',5:'mul',6:'rcp',7:'rsq',8:'dp3',9:'dp4',10:'min',11:'max',12:'slt',13:'sge',14:'exp',15:'log',16:'lit',17:'dst',18:'lrp',19:'frc',20:'m4x4',21:'m4x3',22:'m3x4',23:'m3x3',24:'m3x2',31:'dcl',32:'pow',35:'abs',37:'sincos',66:'texld',81:'def',88:'cmp',90:'dp2add'}
LENGTHS = {0:0,1:2,2:3,3:3,4:4,5:3,6:2,7:2,8:3,9:3,10:3,11:3,12:3,13:3,14:2,15:2,16:2,17:3,18:4,19:2,20:3,21:3,22:3,23:3,24:3,31:2,81:5}
REGS = {0:'r',1:'v',2:'c',3:'t',4:'oRast',5:'oD',6:'oT',7:'i',8:'oC',9:'oDepth',10:'s',14:'b'}


def register(token, destination=False):
    kind = ((token >> 28) & 7) | ((token >> 8) & 24)
    name = REGS[kind] + str(token & 2047)
    if destination:
        mask = ''.join(c for i,c in enumerate('xyzw') if token & (1 << (16+i)))
        return name + '.' + mask + ('_sat' if token & 0x100000 else '')
    swizzle = ''.join('xyzw'[(token >> (16+2*i)) & 3] for i in range(4))
    modifier = (token >> 24) & 15
    assert modifier in (0,1), modifier
    return ('-' if modifier == 1 else '') + name + '.' + swizzle


def decode(data, start):
    version = struct.unpack_from('<I', data, start)[0]
    pc = start + 4
    lines = []
    while pc < len(data):
        token, = struct.unpack_from('<I', data, pc)
        op = token & 65535
        if op == 65535:
            return {'offset':hex(start),'version':hex(version),'end':hex(pc+4),'instructions':lines}
        if op == 65534:
            pc += 4 * (1 + ((token >> 16) & 32767))
            continue
        name = OPS[op]
        count = (token >> 24) & 15 if (version & 0xff00) >= 0x200 else LENGTHS[op]
        args = struct.unpack_from('<'+'I'*count, data, pc+4)
        if op == 81:
            values = [register(args[0],True)] + [repr(struct.unpack('<f',struct.pack('<I',v))[0]) for v in args[1:]]
        elif op == 31:
            values = [hex(args[0]),register(args[1],True)]
        else:
            values = [register(v,i==0) for i,v in enumerate(args)]
        lines.append({'offset':hex(pc),'asm':name+' '+', '.join(values),'tokens':[hex(token),*[hex(v) for v in args]]})
        pc += 4*(1+count)
    raise ValueError('unterminated shader')


def main():
    binary = Path('reverse/FX.dll').read_bytes()
    result = {}
    for resource in json.loads(Path('reverse/evidence/fx-resources.json').read_text()):
        if resource['path'][0] != 10:
            continue
        begin = int(resource['file_offset'],16)
        end = begin + resource['size']
        programs = []
        for pos in range(begin,end-4,4):
            version = struct.unpack_from('<I',binary,pos)[0]
            if version not in (0xfffe0101,0xfffe0200,0xffff0200):
                continue
            if any(int(p['offset'],16) <= pos < int(p['end'],16) for p in programs):
                continue
            try:
                p = decode(binary,pos)
                if p['instructions']:
                    programs.append(p)
            except (KeyError,AssertionError,ValueError,struct.error):
                continue
        result[resource['path'][1]] = programs
    Path('reverse/evidence/fx-bytecode.json').write_text(json.dumps(result,indent=2)+'\n')
    for name,programs in result.items():
        print(name)
        for program in programs:
            print(program['offset'],program['version'])
            print('\n'.join(i['asm'] for i in program['instructions']))

if __name__ == '__main__':
    main()
