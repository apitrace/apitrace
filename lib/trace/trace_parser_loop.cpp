/**************************************************************************
 *
 * Copyright 2014 LunarG, Inc.
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


#include "trace_parser.hpp"


namespace trace {


// Decorator for parser which loops
class LastFrameLoopParser : public AbstractParser  {
public:
    LastFrameLoopParser(AbstractParser *p, int c) {
        parser = p;
        loopCount = c;
        starts_new_frame = true;
    }

    ~LastFrameLoopParser() {
        for (auto c : lastFrameCalls)
            delete c;
        lastFrameCalls.clear();
        delete parser;
    }

    Call *parse_call(void) override;

    // Delegate to Parser
    void getBookmark(ParseBookmark &bookmark) override { parser->getBookmark(bookmark); }
    void setBookmark(const ParseBookmark &bookmark) override { parser->setBookmark(bookmark); }
    bool open(const char *filename) override;
    void close(void) override { parser->close(); }
    unsigned long long getVersion(void) const override { return parser->getVersion(); }
    const Properties & getProperties(void) const override { return parser->getProperties(); }
private:
    int loopCount;
    bool starts_new_frame;
    AbstractParser *parser;
    std::vector<Call *> lastFrameCalls;
    std::vector<Call *>::iterator lastFrameIterator;
};


bool
LastFrameLoopParser::open(const char *filename)
{
    return parser->open(filename);
}

Call *
LastFrameLoopParser::parse_call(void)
{
    trace::Call *call;

    call = parser->parse_call();

    /* Restart last frame when looping is requested. */
    if (call) {
        if (starts_new_frame) {
            for (auto c : lastFrameCalls)
                delete c;
            lastFrameCalls.clear();
        }

        call->reuse_call = true;
        lastFrameCalls.push_back(call);
        starts_new_frame = call->flags & trace::CALL_FLAG_END_FRAME;

        lastFrameIterator = lastFrameCalls.begin();

        return call;
    }

    /* else */
    if (loopCount) {
        call = *lastFrameIterator;
        ++lastFrameIterator;
        if (lastFrameIterator == lastFrameCalls.end()) {
            /* repeat the frame */
            lastFrameIterator = lastFrameCalls.begin();
            --loopCount;
        }
    }

    return call;
}


AbstractParser *
lastFrameLoopParser(AbstractParser *parser, int loopCount)
{
    return new LastFrameLoopParser(parser, loopCount);
}


} /* namespace trace */
