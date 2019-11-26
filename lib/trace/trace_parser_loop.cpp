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
    AbstractParser *parser;
    ParseBookmark lastFrameBegin, lastFrameEnd;
    std::vector<Call *> lastFrameCalls;
    std::vector<Call *>::iterator lastFrameIterator;
};


bool
LastFrameLoopParser::open(const char *filename)
{
    bool ret = parser->open(filename);
    if (ret) {
        /* If the user wants to loop we need to get a bookmark target. We
         * usually get this after replaying a call that ends a frame, but
         * for a trace that has only one frame we need to get it at the
         * beginning. */
        parser->getBookmark(lastFrameBegin);
        lastFrameEnd = lastFrameBegin;
    }
    return ret;
}

Call *
LastFrameLoopParser::parse_call(void)
{
    trace::Call *call;

    call = parser->parse_call();

    /* Restart last frame when looping is requested. */
    if (call) {
        if (call->flags & trace::CALL_FLAG_END_FRAME) {
            lastFrameBegin = lastFrameEnd;
            parser->getBookmark(lastFrameEnd);
        }
        return call;
    }

    /* else */
    if (loopCount) {
        if (lastFrameCalls.empty()) {
            /* initialize vector with calls for the last frame, so we
             * don't have to re-parse when looping. */
            parser->setBookmark(lastFrameBegin);
            while (true) {
                call = parser->parse_call();
                /* prevent call from being cleaned up after retrace.
                 * It sill be retraced manny times. */
                call->reuse_call = true;
                lastFrameCalls.push_back(call);
                ParseBookmark current;
                parser->getBookmark(current);
                if (current.next_call_no == lastFrameEnd.next_call_no) {
                    /* end of frame */
                    lastFrameIterator = lastFrameCalls.begin();
                    break;
                }
            }
        }
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
