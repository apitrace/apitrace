/**************************************************************************
 *
 * Copyright 2012 VMware, Inc.
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


#include <assert.h>
#include <stdlib.h>

#include <limits>
#include <fstream>
#include <iostream>
#include <string>

#include <trace_callset.hpp>

using namespace trace;


// Parser class for call sets
class CallSetParser
{
    CallSet &set;

protected:
    char lookahead;

    CallSetParser(CallSet &_set) :
        set(_set),
        lookahead(0)
    {}

public:
    void parse() {
        skipWhiteSpace();
        while (lookahead) {
            assert(!isSpace());
            parseRange();
            // skip any comma
            isOperator(',');
        }
    }

private:
    void parseRange() {
        CallNo start = std::numeric_limits<CallNo>::min();
        CallNo stop = std::numeric_limits<CallNo>::max();
        CallNo step = 1;
        CallFlags freq = FREQUENCY_ALL;
        if (isAlpha()) {
            freq = parseFrequency();
        } else {
            if (isOperator('*')) {
                // no-change
            } else {
                start = parseCallNo();
                if (isOperator('-')) {
                    if (isDigit()) {
                        stop = parseCallNo();
                    } else {
                        // no-change
                    }
                } else {
                    stop = start;
                }
            }
            if (isOperator('/')) {
                if (isDigit()) {
                    step = parseCallNo();
                } else {
                    freq = parseFrequency();
                }
            }
        }
        set.addRange(CallRange(start, stop, step, freq));
    }

    // match and consume an operator
    bool isOperator(char c) {
        if (lookahead == c) {
            consume();
            skipWhiteSpace();
            return true;
        } else {
            return false;
        }
    }

    CallNo parseCallNo() {
        CallNo number = 0;
        if (isDigit()) {
            do {
                CallNo digit = consume() - '0';
                number = number * 10 + digit;
            } while (isDigit());
        } else {
            std::cerr << "error: expected digit, found '" << lookahead << "'\n";
            exit(0);
        }
        skipWhiteSpace();
        return number;
    }

    CallFlags parseFrequency() {
        std::string freq;
        if (isAlpha()) {
            do {
                freq.push_back(consume());
            } while (isAlpha());
        } else {
            std::cerr << "error: expected frequency, found '" << lookahead << "'\n";
            exit(0);
        }
        skipWhiteSpace();
        if (freq == "frame") {
            return FREQUENCY_FRAME;
        } else if (freq == "rendertarget" || freq == "fbo") {
            return FREQUENCY_RENDERTARGET;
        } else if (freq == "render" || freq == "draw") {
            return FREQUENCY_RENDER;
        } else {
            std::cerr << "error: expected frequency, found '" << freq << "'\n";
            exit(0);
            return FREQUENCY_NONE;
        }
    }

    // match lookahead with a digit (does not consume)
    bool isDigit() const {
        return lookahead >= '0' && lookahead <= '9';
    }

    bool isAlpha() const {
        return lookahead >= 'a' && lookahead <= 'z';
    }

    void skipWhiteSpace() {
        while (isSpace()) {
            consume();
        }
    }

    bool isSpace() const {
        return lookahead == ' ' ||
               lookahead == '\t' ||
               lookahead == '\r' ||
               lookahead == '\n';
    }

    virtual char consume() = 0;
};


class StringCallSetParser : public CallSetParser
{
    const char *buf;

public:
    StringCallSetParser(CallSet &_set, const char *_buf) :
        CallSetParser(_set),
        buf(_buf)
    {
        lookahead = *buf;
    }

    char consume() {
        char c = lookahead;
        if (lookahead) {
            ++buf;
            lookahead = *buf;
        }
        return c;
    }
};


class FileCallSetParser : public CallSetParser
{
    std::ifstream stream;

public:
    FileCallSetParser(CallSet &_set, const char *filename) :
        CallSetParser(_set)
    {
        stream.open(filename);
        if (!stream.is_open()) {
            std::cerr << "error: failed to open \"" << filename << "\"\n";
            exit(1);
        }

        stream.get(lookahead);
    }

    char consume() {
        char c = lookahead;
        if (stream.eof()) {
            lookahead = 0;
        } else {
            stream.get(lookahead);
        }
        return c;
    }
};


CallSet::CallSet(const char *string): limits(std::numeric_limits<CallNo>::min(), std::numeric_limits<CallNo>::max())
{
    if (*string == '@') {
        FileCallSetParser parser(*this, &string[1]);
        parser.parse();
    } else {
        StringCallSetParser parser(*this, string);
        parser.parse();
    }
}


CallSet::CallSet(CallFlags freq): limits(std::numeric_limits<CallNo>::min(), std::numeric_limits<CallNo>::max()) {
    if (freq != FREQUENCY_NONE) {
        CallNo start = std::numeric_limits<CallNo>::min();
        CallNo stop = std::numeric_limits<CallNo>::max();
        CallNo step = 1;
        addRange(CallRange(start, stop, step, freq));
        assert(!empty());
    }
}
