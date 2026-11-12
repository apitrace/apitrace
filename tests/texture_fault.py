"""Inject a driver readback error inside the real Debug texture helper via GDB."""

import argparse
import json
from pathlib import Path
import re
import shutil
import subprocess
import tempfile


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--apitrace', required=True)
    parser.add_argument('--app', required=True)
    parser.add_argument('--retrace', required=True)
    parser.add_argument('--layer', type=int, default=0)
    args = parser.parse_args()
    if not shutil.which('gdb'):
        return 125
    with tempfile.TemporaryDirectory(prefix='apitrace-texture-fault-') as dirname:
        work = Path(dirname)
        trace = work / 'input.trace'
        shape = 'depth-supported' if args.layer < 0 else 'state' if args.layer == 0 else '3d'
        fmt = 'depth' if args.layer < 0 else 'rgba8' if args.layer == 0 else 'rgba16f'
        app = [args.app, 'es31', fmt, shape]
        result = subprocess.run([args.apitrace, 'trace', '-a', 'egl', '-o', str(trace), '--'] + app)
        if result.returncode:
            return result.returncode
        dump = subprocess.check_output([args.apitrace, 'dump', '--grep=glFlush', str(trace)], text=True)
        call = re.findall(r'^\s*(\d+) glFlush', dump, re.MULTILINE)[-1]
        script = work / 'inject.gdb'
        statefile = work / 'state.json'
        errors = work / 'stderr.txt'
        inject = "gdb.execute('set variable context.NV_read_depth_stencil = 0')" if args.layer < 0 else "gdb.execute('set variable width = -1')"
        function = 'glstate::getTexImageOES' if args.layer < 0 else 'glstate::readTexturePixelsOES'
        script.write_text('''set pagination off
set confirm off
set disable-randomization off
python
class InjectReadError(gdb.Breakpoint):
    count = 0
    def stop(self):
        self.count += 1
        if self.count == INJECT_CALL:
            INJECT_ACTION
        return False
inject = InjectReadError('FUNCTION')
end
run --headless -D CALL TRACEFILE > STATEFILE 2> ERRORFILE
python
if inject.count < INJECT_CALL:
    raise RuntimeError('readback breakpoint was not reached')
end
'''.replace('INJECT_CALL', str(max(1, args.layer + 1))).replace('INJECT_ACTION', inject)
            .replace('FUNCTION', function).replace('CALL', call).replace('TRACEFILE', str(trace))
            .replace('STATEFILE', str(statefile)).replace('ERRORFILE', str(errors)))
        result = subprocess.run(['gdb', '--batch', '-x', str(script), '--args', args.retrace,
                                 '--headless', '-D', call, str(trace)], capture_output=True, text=True)
        print(result.stdout)
        print(result.stderr)
        assert result.returncode == 0, 'GDB failed'
        log = errors.read_text()
        expected = 'depth/stencil readback is not supported' if args.layer < 0 else 'GL_INVALID_VALUE'
        assert 'texture readback' in log and expected in log, log
        assert 'was clobbered' not in log, log
        state = json.loads(statefile.read_text(), strict=False)
        target = 'GL_TEXTURE_2D' if args.layer <= 0 else 'GL_TEXTURE_3D'
        key = 'GL_TEXTURE0, %s, level = 0' % target
        assert key not in state['textures'], 'failed image was exported'
        if args.layer >= 0:
            from texture_state import check_textures
            prefix = 'texture_readback_es31_%s_%s.ref.dump.textures.json' % (fmt, shape)
            reference = Path(__file__).parent / 'apps/egl/gles2' / prefix
            expected = json.loads(reference.read_text())
            for i, image in enumerate(expected):
                if image['key'].startswith('GL_TEXTURE0,'):
                    expected[i] = {'key': image['key'], 'absent': True}
            check_textures(state, expected)
        print('PASS: injected readback failure at layer %d; image omitted and state restored' % args.layer)
    return 0


if __name__ == '__main__':
    raise SystemExit(main())
