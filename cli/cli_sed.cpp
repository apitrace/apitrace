/**************************************************************************
 *
 * Copyright 2013 VMware, Inc.
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


#include <limits.h> // for CHAR_MAX
#include <getopt.h>

#include "cli.hpp"

#include "os_string.hpp"

#include "trace_parser.hpp"
#include "trace_writer.hpp"


static const char *synopsis = "Stream editing of a trace.";


static void
usage(void)
{
    std::cout
        << "usage: apitrace sed [OPTIONS] TRACE_FILE...\n"
        << synopsis << "\n"
        "\n"
        "    -h, --help               Show detailed help for sed options and exit\n"
        "    -e s/SEARCH/REPLACE/     Search and replace a symbol.\n"
        "                             XXX: Only works for enums.\n"
        "    -o, --output=TRACE_FILE  Output trace file\n"
    ;
}


const static char *
shortOptions = "ho:e:";

const static struct option
longOptions[] = {
    {"help", no_argument, 0, 'h'},
    {"output", required_argument, 0, 'o'},
    {0, 0, 0, 0}
};

using namespace trace;


/**
 * A visitor that replaces symbol constants.
 */
class Replacer : public Visitor
{
protected:
    std::string searchName;
    std::string replaceName;

public:
    Replacer(const std::string & _searchName, const std::string & _replaceName) :
        searchName(_searchName),
        replaceName(_replaceName)
    {
    }

    ~Replacer() {
    }

    void visit(Null *) {
    }

    void visit(Bool *node) {
    }

    void visit(SInt *node) {
    }

    void visit(UInt *node) {
    }

    void visit(Float *node) {
    }

    void visit(Double *node) {
    }

    void visit(String *node) {
    }

    void visit(Enum *node) {
        const EnumValue *it = node->lookup();
        if (it) {
            if (searchName.compare(it->name) == 0) {
                const EnumSig *sig = node->sig;
                for (unsigned i = 0; i < sig->num_values; ++i) {
                    if (replaceName.compare(sig->values[i].name) == 0) {
                        node->value = sig->values[i].value;
                        break;
                    }
                }
            }
        }
    }

    void visit(Bitmask *bitmask) {
    }

    void visit(Struct *s) {
        for (unsigned i = 0; i < s->members.size(); ++i) {
            Value *memberValue = s->members[i];
            _visit(memberValue);
        }
    }

    void visit(Array *array) {
        for (std::vector<Value *>::iterator it = array->values.begin(); it != array->values.end(); ++it) {
            _visit(*it);
        }
    }

    void visit(Blob *blob) {
    }

    void visit(Pointer *p) {
    }

    void visit(Repr *r) {
        _visit(r->humanValue);
    }

    void visit(Call *call) {
        for (unsigned i = 0; i < call->args.size(); ++i) {
            if (call->args[i].value) {
                _visit(call->args[i].value);
            }
        }

        if (call->ret) {
            _visit(call->ret);
        }
    }
};


typedef std::list<Replacer> Replacements;


static int
sed_trace(Replacements &replacements, const char *inFileName, std::string &outFileName)
{
    trace::Parser p;

    if (!p.open(inFileName)) {
        std::cerr << "error: failed to open " << inFileName << "\n";
        return 1;
    }

    /* Prepare outFileName file and writer for outFileName. */
    if (outFileName.empty()) {
        os::String base(inFileName);
        base.trimExtension();

        outFileName = std::string(base.str()) + std::string("-sed.trace");
    }

    trace::Writer writer;
    if (!writer.open(outFileName.c_str())) {
        std::cerr << "error: failed to create " << outFileName << "\n";
        return 1;
    }

    trace::Call *call;
    while ((call = p.parse_call())) {

        for (Replacements::iterator it = replacements.begin(); it != replacements.end(); ++it) {
            it->visit(call);
        }

        writer.writeCall(call);

        delete call;
    }

    std::cerr << "Edited trace is available as " << outFileName << "\n";

    return 0;
}


/**
 * Parse a string in the format "s/SEARCH/REPLACE/".
 */
static bool
parseSubstOpt(Replacements &replacements, const char *opt)
{
    if (*opt++ != 's') {
        return false;
    }

    if (*opt++ != '/') {
        return false;
    }

    // Parse the search pattern
    const char *search_begin = opt;
    while (*opt != '/') {
        if (*opt == 0) {
            return false;
        }
        ++opt;
    }
    const char *search_end = opt++;

    // Parse the replace pattern
    const char *replace_begin = opt;
    while (*opt != '/') {
        if (*opt == 0) {
            return false;
        }
        ++opt;
    }
    const char *replace_end = opt++;

    if (*opt != 0) {
        return false;
    }

    std::string search(search_begin, search_end);
    std::string replace(replace_begin, replace_end);

    replacements.push_back(Replacer(search, replace));

    return true;
}


static int
command(int argc, char *argv[])
{
    Replacements replacements;
    std::string outFileName;

    int opt;
    while ((opt = getopt_long(argc, argv, shortOptions, longOptions, NULL)) != -1) {
        switch (opt) {
        case 'h':
            usage();
            return 0;
        case 'o':
            outFileName = optarg;
            break;
        case 'e':
            if (!parseSubstOpt(replacements, optarg)) {
                std::cerr << "error: invalid replacement patter `" << optarg << "`\n";
            }
            break;
        default:
            std::cerr << "error: unexpected option `" << (char)opt << "`\n";
            usage();
            return 1;
        }
    }

    if (optind >= argc) {
        std::cerr << "error: apitrace sed requires a trace file as an argument.\n";
        usage();
        return 1;
    }

    if (argc > optind + 1) {
        std::cerr << "error: extraneous arguments:";
        for (int i = optind + 1; i < argc; i++) {
            std::cerr << " " << argv[i];
        }
        std::cerr << "\n";
        usage();
        return 1;
    }

    return sed_trace(replacements, argv[optind], outFileName);
}


const Command sed_command = {
    "sed",
    synopsis,
    usage,
    command
};
