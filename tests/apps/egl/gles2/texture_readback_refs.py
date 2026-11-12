"""Generate references from CPU fixtures, never from eglretrace output."""

import copy
import json
from pathlib import Path
import struct


FORMATS = {
    'rgba8': (4, 'u8', 8), 'r8': (1, 'u8', 8), 'rg8': (2, 'u8', 8),
    'rgba16f': (4, 'float', 16), 'r16f': (1, 'float', 16), 'rg16f': (2, 'float', 16),
    'rgba32f': (4, 'float', 32), 'r32f': (1, 'float', 32), 'rg32f': (2, 'float', 32),
    'rgba8ui': (4, 'ui', 8), 'rgba16ui': (4, 'ui', 16), 'rgba32ui': (4, 'ui', 32),
    'rgba8i': (4, 'i', 8), 'rgba16i': (4, 'i', 16), 'rgba32i': (4, 'i', 32),
    'r32ui': (1, 'ui', 32), 'rg32i': (2, 'i', 32), 'rgb32f': (3, 'float', 32),
    'depth': (1, 'depth', 16), 'depth-stencil': (1, 'depth', 24),
}
FACES = ('POSITIVE_X', 'NEGATIVE_X', 'POSITIVE_Y', 'NEGATIVE_Y', 'POSITIVE_Z', 'NEGATIVE_Z')


def component(fmt, i, channel):
    channels, kind, bits = FORMATS[fmt]
    if channel >= channels:
        return (255 if kind == 'u8' else 1) if channel == 3 else 0
    if kind == 'depth':
        value = (0, 16384, 32768, 65535)[i % 4] / 65535.0 if fmt == 'depth' else float(i % 2)
        return struct.unpack('=f', struct.pack('=f', value))[0]
    if kind == 'float':
        return (-2, -0.5, 0, 0.25, 1, 2, 16, 64)[i % 8]
    if kind == 'ui':
        value = (0, 1, 127, 254, 255, 256, 65535, 0xffffffff)[i % 8]
        return min(255, min((1 << bits) - 1, value))
    if kind == 'i':
        value = (-2147483648, -128, -1, 0, 1, 127, 255, 256, 2147483647)[i % 9]
        value = max(-(1 << (bits - 1)), min((1 << (bits - 1)) - 1, value))
        return max(0, min(255, value))
    return (i * 7 + 31 * channel) % 256


def expectation(profile, fmt, shape):
    channels, kind, _ = FORMATS[fmt]
    cube = shape == 'cube'
    volume = shape.startswith('3d')
    levels = 4 if cube else 3 if 'mip' in shape else 1
    result = []
    for face in range(6 if cube else 1):
        for level in range(levels):
            target = ('GL_TEXTURE_CUBE_MAP_' + FACES[face] if cube else
                      'GL_TEXTURE_3D' if volume else 'GL_TEXTURE_2D')
            unit = 1 if shape == 'mixed-after' else 0
            key = 'GL_TEXTURE%d, %s, level = %d' % (unit, target, level)
            absent = (shape == 'undefined' or
                      kind == 'depth' and shape != 'depth-supported' or
                      fmt == 'rgb32f' and profile != 'desktop')
            if absent:
                image = {'key': key, 'absent': True}
                if kind == 'depth':
                    image['warning'] = 'depth/stencil readback is not supported'
                elif fmt == 'rgb32f':
                    image['warning'] = 'GL_FRAMEBUFFER_'
                result.append(image)
                continue
            width = 8 >> level if cube else max(1, 7 >> level)
            height = width if cube else max(1, 5 >> level)
            depth = max(1, 3 >> level) if volume else 1
            output_channels = 4
            if kind == 'depth':
                output_channels = 1
            elif profile == 'desktop' and channels < 4 and kind in ('u8', 'float'):
                output_channels = 3
            pixels = []
            # Existing Image::flipped output reverses the entire stacked image.
            for z in reversed(range(depth)):
                for y in reversed(range(height)):
                    for x in range(width):
                        pixels.append([
                            component(fmt, x + 3*y + 11*z + 17*face + 23*level + c, c)
                            for c in range(output_channels)
                        ])
            image = {'key': key, 'width': width, 'height': height, 'depth': depth,
                     'type': 'float' if kind in ('float', 'depth') else 'u8', 'pixels': pixels}
            if profile in ('es31', 'desktop'):
                image['format'] = {
                    'depth': 'GL_DEPTH_COMPONENT16', 'depth-stencil': 'GL_DEPTH24_STENCIL8',
                }.get(fmt, 'GL_' + fmt.upper())
            result.append(image)
    if shape in ('mixed', 'mixed-after'):
        empty_unit = 1 if shape == 'mixed' else 0
        result.append({'key': 'GL_TEXTURE%d, GL_TEXTURE_2D, level = 0' % empty_unit,
                       'absent': True})
    if shape in ('state', '3d'):
        duplicate = copy.deepcopy(result)
        for image in duplicate:
            image['key'] = image['key'].replace('GL_TEXTURE0,', 'GL_TEXTURE1,')
        result.extend(duplicate)
    return result


def main():
    root = Path(__file__).resolve().parent
    for profile in ('es20', 'es30', 'es31', 'desktop'):
        cases = [('rgba8', shape) for shape in
                 ('2d', '2d-mip', 'cube', '3d', '3d-mip', 'mixed', 'mixed-after', 'undefined')]
        if profile == 'es20':
            cases += [('rgba16f', '2d'), ('rgba16f', '3d')]
        if profile != 'es20':
            cases += [(fmt, '2d') for fmt in FORMATS if fmt != 'rgba8']
            cases += [('rgba16f', '3d'), ('rgba32ui', '3d'), ('rgba32i', '3d'),
                      ('rgba16f', '3d-mip'), ('rgba32i', '3d-mip'), ('rgba8', 'state')]
            if profile != 'desktop':
                cases += [('depth', 'depth-supported'), ('depth-stencil', 'depth-supported')]
        for fmt, shape in cases:
            if profile == 'desktop' and fmt in ('depth', 'depth-stencil'):
                continue
            name = 'texture_readback_%s_%s_%s' % (profile, fmt.replace('-', '_'), shape)
            (root / (name + '.ref.txt')).write_text(
                '//!texture_readback %s %s %s\n<dump> glFlush()\n' % (profile, fmt, shape))
            (root / (name + '.ref.dump.textures.json')).write_text(
                json.dumps(expectation(profile, fmt, shape), separators=(',', ':')) + '\n')


if __name__ == '__main__':
    main()
