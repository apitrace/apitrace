import base64
import struct
import unittest

from texture_state import check_textures, decode_image


class TextureStateTest(unittest.TestCase):
    def test_float_values_are_not_quantized(self):
        payload = b'PX\n1 1\n1\n' + struct.pack('=4f', -2, 0.25, 2, 64)
        obj = {'__data__': base64.b64encode(payload).decode()}
        self.assertEqual(decode_image(obj), (1, 1, 'float', [[-2, 0.25, 2, 64]]))

    def test_truncated_float_image(self):
        obj = {'__data__': base64.b64encode(b'PX\n1 1\n1\n' + b'\0' * 4).decode()}
        with self.assertRaises(AssertionError):
            decode_image(obj)

    def test_failure_must_omit_image(self):
        with self.assertRaises(AssertionError):
            check_textures({'textures': {'texture': {}}}, [{'key': 'texture', 'absent': True}])

    def test_missing_image_fails(self):
        with self.assertRaises(AssertionError):
            check_textures({'textures': {}}, [{'key': 'texture'}])


if __name__ == '__main__':
    unittest.main()
