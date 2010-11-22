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
import gl



class ConstRemover(base.Rebuilder):

    def visit_const(self, const):
        return const.type

    def visit_opaque(self, opaque):
        expr = opaque.expr
        if expr.startswith('const '):
            expr = expr[6:]
        return base.Opaque(expr)


class ValueExtractor(base.Visitor):

    def visit_literal(self, literal, lvalue, rvalue):
        print '    %s = %s;' % (lvalue, rvalue)

    def visit_alias(self, alias, lvalue, rvalue):
        self.visit(alias.type, lvalue, rvalue)
    
    def visit_enum(self, enum, lvalue, rvalue):
        print '    %s = %s;' % (lvalue, rvalue)

    def visit_bitmask(self, bitmask, lvalue, rvalue):
        self.visit(bitmask.type, lvalue, rvalue)

    def visit_array(self, array, lvalue, rvalue):
        print '    const Trace::Array *__a%s = dynamic_cast<const Trace::Array *>(&%s);' % (array.id, rvalue)
        print '    if (__a%s) {' % (array.id)
        length = '__a%s->values.size()' % array.id
        print '        %s = new %s[%s];' % (lvalue, array.type, length)
        index = '__i' + array.id
        print '        for(size_t {i} = 0; {i} < {length}; ++{i}) {{'.format(i = index, length = length)
        try:
            self.visit(array.type, '%s[%s]' % (lvalue, index), '*__a%s->values[%s]' % (array.id, index))
        finally:
            print '        }'
            print '    } else {'
            print '        %s = NULL;' % lvalue
            print '    }'

    def visit_blob(self, blob, lvalue, rvalue):
        print '    %s = static_cast<%s>((%s).blob());' % (lvalue, blob, rvalue)
    
    def visit_string(self, string, lvalue, rvalue):
        print '    %s = (%s).string();' % (lvalue, rvalue)



def retrace_function(function):
    print 'static void retrace_%s(Trace::Call &call) {' % function.name
    success = True
    for arg in function.args:
        arg.type = ConstRemover().visit(arg.type)
        print '    %s %s;' % (arg.type, arg.name)
        rvalue = 'call.arg("%s")' % (arg.name,)
        lvalue = arg.name
        try:
            ValueExtractor().visit(arg.type, lvalue, rvalue)
        except NotImplementedError:
            success = False
            print '    %s = 0; // FIXME' % arg.name
    if not success:
        print '    std::cerr << "warning: unsupported call %s\\n";' % function.name
        print '    return;'
    arg_names = ", ".join([arg.name for arg in function.args])
    print '    %s(%s);' % (function.name, arg_names)
    print '}'
    print


def retrace_functions(functions):
    for function in functions:
        if function.sideeffects:
            retrace_function(function)

    print 'static bool retrace_call(Trace::Call &call) {'
    for function in functions:
        if not function.sideeffects:
            print '    if (call.name == "%s") {' % function.name
            print '        return true;'
            print '    }'
    print
    print '    std::cout << call;'
    print '    std::cout.flush();'
    print
    for function in functions:
        if function.sideeffects:
            print '    if (call.name == "%s") {' % function.name
            print '        retrace_%s(call);' % function.name
            print '        return true;'
            print '    }'
    print '    std::cerr << "warning: unsupported call " << call.name << "\\n";'
    print '    return false;'
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

    functions = gl.basic_functions(base.Function) + gl.extended_functions(base.Function)
    retrace_functions(functions)

    print '''

class Retracer : public Trace::Parser
{
    void handle_call(Trace::Call &call) {
        if (call.name == "wglSwapBuffers" ||
            call.name == "glXSwapBuffers") {
            glFlush();
            return;
        }
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
