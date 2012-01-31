/**************************************************************************
 *
 * Copyright 2012 Jose Fonseca
 * All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 **************************************************************************/


#include <string.h>

#ifdef _WIN32
#include <fcntl.h>
#include <io.h>
#endif

#include "pickle.hpp"

#include "cli.hpp"
#include "cli_pager.hpp"

#include "trace_parser.hpp"
#include "trace_model.hpp"
#include "trace_callset.hpp"


using namespace trace;


class PickleVisitor : public trace::Visitor
{
protected:
    PickleWriter &writer;

public:
    PickleVisitor(PickleWriter &_writer) :
        writer(_writer) {
    }

    void visit(Null *node) {
        writer.writeInt(0);
    }

    void visit(Bool *node) {
        writer.writeBool(node->value);
    }

    void visit(SInt *node) {
        writer.writeInt(node->value);
    }

    void visit(UInt *node) {
        writer.writeInt(node->value);
    }

    void visit(Float *node) {
        writer.writeFloat(node->value);
    }

    void visit(Double *node) {
        writer.writeFloat(node->value);
    }

    void visit(String *node) {
        writer.writeString(node->value);
    }

    void visit(Enum *node) {
        // TODO: keep symbolic name
        writer.writeInt(node->value);
    }

    void visit(Bitmask *node) {
        // TODO: keep symbolic name
        writer.writeInt(node->value);
    }

    void visit(Struct *node) {
        writer.beginDict();
        for (unsigned i = 0; i < node->sig->num_members; ++i) {
            writer.beginItem(node->sig->member_names[i]);
            _visit(node->members[i]);
            writer.endItem();
        }
        writer.endDict();
    }

    void visit(Array *node) {
        writer.beginList();
        for (std::vector<Value *>::iterator it = node->values.begin(); it != node->values.end(); ++it) {
            _visit(*it);
        }
        writer.endList();
    }

    void visit(Blob *node) {
        writer.writeString((const char *)node->buf, node->size);
    }

    void visit(Pointer *node) {
        writer.writeInt(node->value);
    }

    void visit(Call *call) {
        writer.beginTuple();

        writer.writeInt(call->no);

        writer.writeString(call->name());

        writer.beginList();
        for (unsigned i = 0; i < call->args.size(); ++i) {
            if (call->args[i].value) {
                _visit(call->args[i].value);
            } else {
                writer.writeNone();
            }
        }
        writer.endList();

        if (call->ret) {
            _visit(call->ret);
        } else {
            writer.writeNone();
        }

        writer.endTuple();
    }
};


static trace::CallSet calls(trace::FREQUENCY_ALL);

static const char *synopsis = "Pickle given trace(s) to standard output.";

static void
usage(void)
{
    std::cout
        << "usage: apitrace pickle [OPTIONS] <trace-file>...\n"
        << synopsis << "\n"
        "\n"
        "    --calls <CALLSET>   Only pickle specified calls\n"
    ;
}

static int
command(int argc, char *argv[])
{
    int i;

    for (i = 1; i < argc;) {
        const char *arg = argv[i];

        if (arg[0] != '-') {
            break;
        }

        ++i;

        if (!strcmp(arg, "--")) {
            break;
        } else if (!strcmp(arg, "--help")) {
            usage();
            return 0;
        } else if (!strcmp(arg, "--calls")) {
            calls = trace::CallSet(argv[i++]);
        } else {
            std::cerr << "error: unknown option " << arg << "\n";
            usage();
            return 1;
        }
    }

#ifdef _WIN32
    // Set stdout in binary mode
    fflush(stdout);
    int mode = _setmode(_fileno(stdout), _O_BINARY);
    if (mode == -1) {
        std::cerr << "warning: failed to set stdout in binary mode\n";
    }
#endif

    for (; i < argc; ++i) {
        trace::Parser parser;

        if (!parser.open(argv[i])) {
            std::cerr << "error: failed to open " << argv[i] << "\n";
            return 1;
        }

        trace::Call *call;
        while ((call = parser.parse_call())) {
            if (calls.contains(*call)) {
                PickleWriter writer(std::cout);
                PickleVisitor visitor(writer);
                
                visitor.visit(call);
            }
            delete call;
        }
    }

#ifdef _WIN32
    std::cout.flush();
    fflush(stdout);
    if (mode != -1) {
        _setmode(_fileno(stdout), mode);
    }
#endif

    return 0;
}

const Command pickle_command = {
    "pickle",
    synopsis,
    usage,
    command
};
