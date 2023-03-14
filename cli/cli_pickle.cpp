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
#include <limits.h> // for CHAR_MAX
#include <getopt.h>

#include "pickle.hpp"

#include "os_binary.hpp"

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
    bool symbolic;

public:
    PickleVisitor(PickleWriter &_writer, bool _symbolic) :
        writer(_writer),
        symbolic(_symbolic) {
    }

    void visit(Null *node) override {
        writer.writeInt(0);
    }

    void visit(Bool *node) override {
        writer.writeBool(node->value);
    }

    void visit(SInt *node) override {
        writer.writeInt(node->value);
    }

    void visit(UInt *node) override {
        writer.writeInt(node->value);
    }

    void visit(Float *node) override {
        writer.writeFloat(node->value);
    }

    void visit(Double *node) override {
        writer.writeFloat(node->value);
    }

    void visit(String *node) override {
        writer.writeString(node->value);
    }

    void visit(WString *node) override {
        writer.writeWString(node->value);
    }

    void visit(Enum *node) override {
        if (symbolic) {
            const EnumValue *it = node->lookup();
            if (it) {
                writer.writeString(it->name);
                return;
            }
        }
        writer.writeInt(node->value);
    }

    void visit(Bitmask *node) override {
        if (symbolic) {
            unsigned long long value = node->value;
            const BitmaskSig *sig = node->sig;
            writer.beginTuple();
            for (const BitmaskFlag *it = sig->flags; it != sig->flags + sig->num_flags; ++it) {
                if ((it->value && (value & it->value) == it->value) ||
                    (!it->value && value == 0)) {
                    writer.writeString(it->name);
                    value &= ~it->value;
                }
                if (value == 0) {
                    break;
                }
            }
            if (value) {
                writer.writeInt(value);
            }
            writer.endTuple();
        } else {
            writer.writeInt(node->value);
        }
    }

    void visit(Struct *node) override {
        if (true) {
            // Structures as dictionaries
            writer.beginDict();
            for (unsigned i = 0; i < node->sig->num_members; ++i) {
                writer.beginItem(node->sig->member_names[i]);
                _visit(node->members[i]);
                writer.endItem();
            }
            writer.endDict();
        } else {
            // Structures as tuples
            unsigned num_members = node->sig->num_members;
            writer.beginTuple(num_members);
            for (unsigned i = 0; i < num_members; ++i) {
                _visit(node->members[i]);
            }
            writer.endTuple(num_members);
        }
    }

    void visit(Array *node) override {
        writer.beginList();
        for (auto & value : node->values) {
            _visit(value);
        }
        writer.endList();
    }

    void visit(Blob *node) override {
        writer.writeBytes(node->buf, node->size);
    }

    void visit(Pointer *node) override {
        writer.writePointer(node->value);
    }

    void visit(Repr *r) override {
        if (symbolic) {
            _visit(r->humanValue);
        } else {
            _visit(r->machineValue);
        }
    }

    void visit(Call *call) {
        writer.beginTuple();

        writer.writeInt(call->no);

        writer.writeInt(call->thread_id);

        writer.writeString(call->name());

        writer.beginList();
        for (unsigned i = 0; i < call->args.size(); ++i) {
            writer.beginTuple(2);
            if (i < call->sig->num_args) {
                writer.writeString(call->sig->arg_names[i]);
            } else {
                writer.writeNone();
            }
            if (call->args[i].value) {
                _visit(call->args[i].value);
            } else {
                writer.writeNone();
            }
            writer.endTuple(2);
        }
        writer.endList();

        if (call->ret) {
            _visit(call->ret);
        } else {
            writer.writeNone();
        }

        writer.writeInt(call->flags);

        writer.endTuple();
    }
};


static trace::CallSet calls(trace::FREQUENCY_ALL);

static const char *synopsis = "Pickle given trace(s) to standard output.";

static void
usage(void)
{
    std::cout
        << "usage: apitrace pickle [OPTIONS] TRACE_FILE...\n"
        << synopsis << "\n"
        "\n"
        "    -h, --help           show this help message and exit\n"
        "    -s, --symbolic       dump symbolic names\n"
        "    --calls=CALLSET      only dump specified calls\n"
    ;
}

enum {
	CALLS_OPT = CHAR_MAX + 1,
};

const static char *
shortOptions = "hs";

const static struct option
longOptions[] = {
    {"help", no_argument, 0, 'h'},
    {"symbolic", no_argument, 0, 's'},
    {"calls", required_argument, 0, CALLS_OPT},
    {0, 0, 0, 0}
};

static int
command(int argc, char *argv[])
{
    bool symbolic = false;

    int opt;
    while ((opt = getopt_long(argc, argv, shortOptions, longOptions, NULL)) != -1) {
        switch (opt) {
        case 'h':
            usage();
            return 0;
        case 's':
            symbolic = true;
            break;
        case CALLS_OPT:
            calls.merge(optarg);
            break;
        default:
            std::cerr << "error: unexpected option `" << (char)opt << "`\n";
            usage();
            return 1;
        }
    }

    os::setBinaryMode(stdout);
    
    std::cout.sync_with_stdio(false);

    PickleWriter writer(std::cout);
    PickleVisitor visitor(writer, symbolic);

    for (int i = optind; i < argc; ++i) {
        trace::Parser parser;

        if (!parser.open(argv[i])) {
            return 1;
        }

        trace::Call *call;
        while ((call = parser.parse_call())) {
            if (call->no > calls.getLast()) {
                delete call;
                break;
            }
            if (calls.contains(*call)) {
                writer.begin();
                visitor.visit(call);
                writer.end();
            }
            delete call;
        }
    }

    return 0;
}

const Command pickle_command = {
    "pickle",
    synopsis,
    usage,
    command
};
