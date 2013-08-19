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

#ifndef _FORMATTER_HPP_
#define _FORMATTER_HPP_


#include <iostream>

#ifdef _WIN32
#include <windows.h>

#ifndef COMMON_LVB_LEADING_BYTE
#define COMMON_LVB_LEADING_BYTE    0x0100
#endif

#ifndef COMMON_LVB_TRAILING_BYTE
#define COMMON_LVB_TRAILING_BYTE   0x0200
#endif

#ifndef COMMON_LVB_GRID_HORIZONTAL
#define COMMON_LVB_GRID_HORIZONTAL 0x0400
#endif

#ifndef COMMON_LVB_GRID_LVERTICAL
#define COMMON_LVB_GRID_LVERTICAL  0x0800
#endif

#ifndef COMMON_LVB_GRID_RVERTICAL
#define COMMON_LVB_GRID_RVERTICAL  0x1000
#endif

#ifndef COMMON_LVB_REVERSE_VIDEO
#define COMMON_LVB_REVERSE_VIDEO   0x4000
#endif

#ifndef COMMON_LVB_UNDERSCORE
#define COMMON_LVB_UNDERSCORE      0x8000
#endif

#endif /* _WIN32 */


namespace formatter {

/*
 * See also http://bytes.com/topic/c/answers/63822-design-question-little-c-header-colorizing-text-linux-comments-ideas
 */

class Attribute {
public:
    virtual ~Attribute() {}

    virtual void apply(std::ostream &) const {}
};


enum Color {
    RED,
    GREEN,
    BLUE,
};


class Formatter {
public:
    virtual ~Formatter() {}

    virtual Attribute *normal(void) const { return new Attribute; }
    virtual Attribute *bold(void) const { return new Attribute; }
    virtual Attribute *italic(void) const { return new Attribute; }
    virtual Attribute *strike(void) const { return new Attribute; }
    virtual Attribute *color(Color) const { return new Attribute; }
};


class AnsiAttribute : public Attribute {
protected:
    const char *escape;
public:
    AnsiAttribute(const char *_escape) : escape(_escape) {}
    void apply(std::ostream& os) const {
        os << "\33[" << escape;
    }
};


/**
 * Formatter for plain-text files which outputs ANSI escape codes. See
 * http://en.wikipedia.org/wiki/ANSI_escape_code for more information
 * concerning ANSI escape codes.
 */
class AnsiFormatter : public Formatter {
protected:
public:
    virtual Attribute *normal(void) const { return new AnsiAttribute("0m"); }
    virtual Attribute *bold(void) const { return new AnsiAttribute("1m"); }
    /* Italic is not widely supported, or worse, implemented with a reverse */
#if 0
    virtual Attribute *italic(void) const { return new AnsiAttribute("3m"); }
#endif
    virtual Attribute *strike(void) const { return new AnsiAttribute("9m"); }
    virtual Attribute *color(Color c) const { 
        static const char *color_escapes[] = {
            "31m", /* red */
            "32m", /* green */
            "34m", /* blue */
        };
        return new AnsiAttribute(color_escapes[c]); 
    }
};


inline std::ostream& operator<<(std::ostream& os, const Attribute *attr) {
    attr->apply(os);
    return os;
}


#ifdef _WIN32


class WindowsAttribute : public Attribute {
protected:
    WORD wAttributes;
public:
    WindowsAttribute(WORD _wAttributes) : wAttributes(_wAttributes) {}
    void apply(std::ostream& os) const {
        DWORD nStdHandleOutput;
        if (&os == &std::cout) {
            nStdHandleOutput = STD_OUTPUT_HANDLE;
        } else if (&os == &std::cerr) {
            nStdHandleOutput = STD_ERROR_HANDLE;
        } else {
            return;
        }
        HANDLE hConsoleOutput = GetStdHandle(nStdHandleOutput);
        if (hConsoleOutput == INVALID_HANDLE_VALUE) {
            return;
        }

        SetConsoleTextAttribute(hConsoleOutput, wAttributes);
    }
};


/**
 * Formatter for the Windows Console.
 */
class WindowsFormatter : public Formatter {
protected:
public:
    virtual Attribute *normal(void) const { return new WindowsAttribute(FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED); }
    virtual Attribute *bold(void) const { return new WindowsAttribute(FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY); }
    virtual Attribute *italic(void) const { return new WindowsAttribute(FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED); }
    virtual Attribute *strike(void) const { return new WindowsAttribute(COMMON_LVB_REVERSE_VIDEO | FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED); }
    virtual Attribute *color(Color c) const { 
        static const WORD color_escapes[] = {
            FOREGROUND_RED | FOREGROUND_INTENSITY,
            FOREGROUND_GREEN | FOREGROUND_INTENSITY,
            FOREGROUND_BLUE | FOREGROUND_INTENSITY, 
        };
        return new WindowsAttribute(color_escapes[c]); 
    }
};


#endif /* _WIN32 */


inline Formatter *defaultFormatter(bool color = true) {
    if (color) {
#ifdef _WIN32
        // http://wiki.winehq.org/DeveloperFaq#detect-wine
        static HMODULE hNtDll = NULL;
        static bool bWine = false;
        if (!hNtDll) {
            hNtDll = LoadLibraryA("ntdll");
            if (hNtDll) {
                bWine = GetProcAddress(hNtDll, "wine_get_version") != NULL;
            }
        }
        if (bWine) {
            return new AnsiFormatter;
        }
        return new WindowsFormatter;
#else
        return new AnsiFormatter;
#endif
    } else {
        return new Formatter;
    }
}


} /* namespace formatter */


#endif /* _FORMATTER_HPP_ */
