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



class ConstRemover(base.Rebuilder):

    def visit_const(self, const):
        return const.type


class ValueExtractor(base.Visitor):

    def visit_literal(self, type, lvalue, rvalue):
        print '    %s = %s;' % (lvalue, rvalue)

    def visit_alias(self, type, lvalue, rvalue):
        self.visit(type.type, lvalue, rvalue)
    
    def visit_enum(self, type, lvalue, rvalue):
        print '    %s = %s;' % (lvalue, rvalue)

    def visit_bitmask(self, type, lvalue, rvalue):
        self.visit(type.type, lvalue, rvalue)

    def visit_array(self, array, lvalue, rvalue):
        print '    %s = new %s[%s];' % (lvalue, array.type, array.length)
        index = '__i' + array.id
        print '    for(size_t {i} = 0; {i} < {length}; ++{i}) {{'.format(i = index, length = array.length)
        self.visit(array.type, '%s[%s]' % (lvalue, index), '%s[%s]' % (rvalue, index))
        print '    }'



def retrace_function(function):
    print 'static void retrace_%s(Trace::Call &call) {' % function.name
    if not function.name.startswith('glX'):
        success = True
        for arg_type, arg_name in function.args:
            arg_type = ConstRemover().visit(arg_type)
            print '    %s %s;' % (arg_type, arg_name)
            rvalue = 'call.arg("%s")' % (arg_name,)
            lvalue = arg_name
            try:
                ValueExtractor().visit(arg_type, lvalue, rvalue)
            except NotImplementedError:
                success = False
                print '    %s = 0; // FIXME' % arg_name
        if not success:
            print '    std::cerr << "warning: unsupported call %s\\n";' % function.name
            print '    return;'
        arg_names = ", ".join([arg_name for arg_type, arg_name in function.args])
        print '    %s(%s);' % (function.name, arg_names)
    print '}'
    print


if __name__ == '__main__':
    print
    print '#include <stdlib.h>'
    print '#include <string.h>'
    print '#include <GL/glew.h>'
    print '#include <GL/glut.h>'
    print
    print '#include "trace_parser.hpp"'
    print

    for function in libgl.functions:
        retrace_function(function)

    print 'static bool retrace_call(Trace::Call &call) {'
    for function in libgl.functions:
        print '    if (call.name == "%s") {' % function.name
        print '        retrace_%s(call);' % function.name
        print '        return true;'
        print '    }'
    print '    std::cerr << "warning: unsupported call " << call.name << "\\n";'
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
