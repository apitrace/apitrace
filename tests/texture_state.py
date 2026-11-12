"""Numerical checks for texture images in dump-state (no PNG float conversion)."""

import base64
import io
import json
import struct


def decode_image(obj):
    data = base64.b64decode(obj['__data__'])
    if data.startswith(b'\x89PNG'):
        from PIL import Image
        im = Image.open(io.BytesIO(data))
        return im.width, im.height, 'u8', [list(p) if isinstance(p, tuple) else [p] for p in im.getdata()]
    stream = io.BytesIO(data)
    magic = stream.readline().strip()
    channels = {b'Pf': 1, b'PF': 3, b'PX': 4}[magic]
    line = stream.readline()
    while line.startswith(b'#'):
        line = stream.readline()
    width, height = map(int, line.split())
    stream.readline()  # Existing apitrace PNM stores native floats, including PX.
    payload = stream.read()
    count = width * height * channels
    if len(payload) != count * 4:
        raise AssertionError('invalid float image payload size')
    values = struct.unpack('=%df' % count, payload)
    return width, height, 'float', [list(values[i:i + channels]) for i in range(0, count, channels)]


def check_textures(state, reference):
    textures = state['textures']
    for expected in reference:
        key = expected['key']
        if expected.get('absent'):
            assert key not in textures, '%s unexpectedly exported' % key
            continue
        assert key in textures, '%s missing' % key
        obj = textures[key]
        assert obj.get('__class__') == 'image', '%s is not an image' % key
        for name in ('width', 'height', 'depth', 'format'):
            if name in expected:
                assert obj['__%s__' % name] == expected[name], '%s %s: %r != %r' % (
                    key, name, obj['__%s__' % name], expected[name])
        width, height, kind, pixels = decode_image(obj)
        assert (width, height) == (expected['width'], expected['height'] * expected['depth']), key
        assert kind == expected['type'], '%s type: %s' % (key, kind)
        assert len(pixels) == len(expected['pixels']), '%s pixel count' % key
        for i, (actual, wanted) in enumerate(zip(pixels, expected['pixels'])):
            assert actual == wanted, '%s pixel (%d,%d): %r != %r' % (
                key, i % width, i // width, actual, wanted)


def check_file(state, filename, diagnostics=None):
    with open(filename) as stream:
        reference = json.load(stream)
    check_textures(state, reference)
    if diagnostics is not None:
        for expected in reference:
            if 'warning' in expected:
                assert expected['warning'] in diagnostics, 'missing diagnostic: %s' % expected['warning']
