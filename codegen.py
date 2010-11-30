##########################################################################
#
# Copyright 2010 VMware, Inc.
# All Rights Reserved.
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
#
##########################################################################/


"""C code generation helpers."""


def _string_switch(var, prefix, suffixes, case, default):
    index = len(prefix)
    indent = '    '*(index + 1)

    if len(suffixes) == 1:
        suffix = suffixes[0]
        chars = list(suffix) + ["\\0"]
        predicates = ['%s[%u] == \'%s\'' % (var, index + i, chars[i]) for i in range(len(chars))]
        print indent + 'if (%s) {' % (' && '.join(predicates),)
        value = prefix + suffix
        print indent + '    // %s' % value
        case(value)
        print indent + '}'
        if default is not None:
            print indent + 'else {'
            default()
            print indent + '}'
        return

    print indent + 'switch (%s[%u]) {' % (var, index)
    if "" in suffixes:
        print indent + 'case \'\\0\':'
        print indent + '    // %s' % prefix
        case(prefix)
        print indent + '    break;'

    chars = [suffix[0] for suffix in suffixes if suffix]
    chars = list(set(chars))
    chars.sort()

    for char in chars:
        print indent + 'case \'%c\':' % (char)
        new_prefix = prefix + char
        new_suffixes = [suffix[1:] for suffix in suffixes if suffix.startswith(char)]
        _string_switch(var, new_prefix, new_suffixes, case, default)
        print indent + '    break;'
    if default is not None:
        print indent + 'default:'
        default()
        print indent + '    break;'
    print indent + '}'


def _noop(*args, **kwargs):
    pass


def string_switch(var, values, case=_noop, default=None):
    """Product a switch of strings, using a tree character switches."""
    values = list(values)
    values.sort()
    _string_switch(var, '', values, case, default)
