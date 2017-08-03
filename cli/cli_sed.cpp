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

#include <string>

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
        "    -e s/SEARCH/REPLACE/     Search and replace a symbol. Use @file(<path>)\n"
        "                             to read SEARCH or REPLACE from a file.\n"
        "                             Any character (not just /) can be used as \n"
        "                             separator.\n"
        "                             XXX: Only works for enums and strings.\n"
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

    void visit(Null *) override {
    }

    void visit(Bool *node) override {
    }

    void visit(SInt *node) override {
    }

    void visit(UInt *node) override {
    }

    void visit(Float *node) override {
    }

    void visit(Double *node) override {
    }

    void visit(String *node) override {
        if (!searchName.compare(node->value)) {
            size_t len = replaceName.length() + 1;
            delete [] node->value;
            char *str = new char [len];
            memcpy(str, replaceName.c_str(), len);
            node->value = str;
        }
    }

    void visit(WString *node) override {
    }

    void visit(Enum *node) override {
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

    void visit(Bitmask *bitmask) override {
    }

    void visit(Struct *s) override {
        for (auto memberValue : s->members) {
            _visit(memberValue);
        }
    }

    void visit(Array *array) override {
        for (auto & value : array->values) {
            _visit(value);
        }
    }

    void visit(Blob *blob) override {
    }

    void visit(Pointer *p) override {
    }

    void visit(Repr *r) override {
        _visit(r->humanValue);
    }

    void visit(Call *call) {
        for (auto & arg : call->args) {
            if (arg.value) {
                _visit(arg.value);
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
    if (!writer.open(outFileName.c_str(), p.getVersion(), p.getProperties())) {
        std::cerr << "error: failed to create " << outFileName << "\n";
        return 1;
    }

    trace::Call *call;
    while ((call = p.parse_call())) {

        for (auto & replacement : replacements) {
            replacement.visit(call);
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
    char separator;

    if (*opt++ != 's') {
        return false;
    }

    separator = *opt++;

    // Parse the search pattern
    const char *search_begin = opt;
    while (*opt != separator) {
        if (*opt == 0) {
            return false;
        }
        ++opt;
    }
    const char *search_end = opt++;

    // Parse the replace pattern
    const char *replace_begin = opt;
    while (*opt != separator) {
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

    // If search or replace strings are taken from a file, read the file
    std::string file_subst = "@file(";

    for (int i = 0; i < 2; i++) {
        std::string *str = i ? &search : &replace;

        if (!str->compare(0, file_subst.length(), file_subst)) {
            if ((*str)[str->length()-1] != ')') {
                return false;
            }

            std::string fname = str->substr(file_subst.length());
            fname[fname.length()-1] = 0;
            FILE *f = fopen(fname.c_str(), "rt");
            if (!f) {
                std::cerr << "error: cannot open file " << fname << "\n";
                return false;
            }
            char buf[1024];
            (*str) = "";
            while (!feof(f)) {
                if (fgets(buf, 1024, f)) {
                    str->append(buf);
                }
            }
            fclose(f);
        }
    }


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
                std::cerr << "error: invalid replacement pattern `" << optarg << "`\n";
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
