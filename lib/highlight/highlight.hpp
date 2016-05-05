/**************************************************************************
 *
 * Copyright 2010 VMware, Inc.
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

/*
 * Helpers for coloring output.
 */

#pragma once


#include <iostream>


namespace highlight {


/*
 * See also http://bytes.com/topic/c/answers/63822-design-question-little-c-header-colorizing-text-linux-comments-ideas
 */

class Attribute {
public:
    virtual void apply(std::ostream &) const = 0;
};


inline std::ostream &
operator << (std::ostream & os, const Attribute & attr) {
    attr.apply(os);
    return os;
}


enum Color {
    RED,
    GREEN,
    BLUE,
    GRAY,
};


class Highlighter {
public:
    virtual const Attribute & normal(void) const = 0;
    virtual const Attribute & bold(void) const = 0;
    virtual const Attribute & italic(void) const = 0;
    virtual const Attribute & strike(void) const = 0;
    virtual const Attribute & color(Color) const = 0;
};


bool
isAtty(std::ostream & os);

const Highlighter &
defaultHighlighter(bool color = true);

const Highlighter &
defaultHighlighter(std::ostream & os);


} /* namespace highlight */


