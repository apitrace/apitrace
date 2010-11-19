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


import base
from glx import libgl


def is_arg_supported(arg_type):
    if isinstance(arg_type, (base.Literal, base._String, base.Enum)):
        return True
    if isinstance(arg_type, (base.Alias, base.Flags)):
        return is_arg_supported(arg_type.type)
    return False


def is_function_supported(function):
    for arg_type, arg_name in function.args:
        if not is_arg_supported(arg_type):
            return False
    return True


def extract_value(arg_type, arg_value):
    if isinstance(arg_type, base.Literal):
        return 'Trace::as%s(%s)' % (arg_type.format, value)
    if isinstance(arg_type, base.Enum):
        return 'Trace::asSInt(%s)' % (value)
    if isinstance(arg_type, (base.Alias, base.Flags)):
        return extract_value(arg_type.type, value)
    assert false
    return '0'


if __name__ == '__main__':
    print
    print '#include <stdlib.h>'
    print '#include <string.h>'
    print '#include <GL/glew.h>'
    print '#include <GL/glut.h>'
    print
    print '#include "trace_parser.hpp"'
    print

    functions = filter(is_function_supported, libgl.functions)
   
    for function in functions:
        print 'static void retrace_%s(Trace::Call &call) {' % function.name
        for arg_type, arg_name in function.args:
            print '    %s %s;' % (arg_type, arg_name)
        for arg_type, arg_name in function.args:
            value = 'call.get_arg("%s")' % (arg_name,)
            value = extract_value(arg_type, value)
            print '    %s = static_cast<%s>(%s);' % (arg_name, arg_type, value)
        arg_names = ", ".join([arg_name for arg_type, arg_name in function.args])
        print '    %s(%s);' % (function.name, arg_names)
        print '}'
        print

    print 'static bool retrace_call(Trace::Call &call) {'
    for function in functions:
        print '    if (call.name == "%s") {' % function.name
        print '        retrace_%s(call);' % function.name
        print '        return true;'
        print '    }'
    print '    std::cerr << "Unsupported call " << call.name << "\\n";'
    print '    return false;'
    print '}'
    print '''

class Retracer : public Trace::Parser
{
    void handle_call(Trace::Call &call) {
        std::cout << call;
        std::cout.flush();
        retrace_call(call);
    }
};

int main(int argc, char **argv)
{
   glutInit(&argc, argv);
   glutInitWindowPosition( 0, 0 );
   glutInitWindowSize( 800, 600 );
   glutInitDisplayMode( GLUT_DEPTH | GLUT_RGB | GLUT_SINGLE );
   glutCreateWindow(argv[0]);
   glewInit();
   for (int i = 1; i < argc; ++i) {
      Retracer p;
      p.parse(argv[i]);
      glutMainLoop();
   }
   return 0;
}

'''   
