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


#ifdef _WIN32

#include <windows.h>
#include <io.h> // _isatty
#include <stdio.h> // _fileno

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

#else /* !_WIN32 */

#include <unistd.h> // isatty

#endif /* !_WIN32 */


#include "highlight.hpp"


namespace highlight {


class PlainAttribute : public Attribute {
public:
    PlainAttribute(void) {}
    virtual void apply(std::ostream &) const override {}
};

static const PlainAttribute plainAttribute;


class PlainHighlighter : public Highlighter {
public:
    PlainHighlighter(void) {}
    virtual const Attribute & normal(void) const override { return plainAttribute; }
    virtual const Attribute & bold(void) const override { return plainAttribute; }
    virtual const Attribute & italic(void) const override { return plainAttribute; }
    virtual const Attribute & strike(void) const override { return plainAttribute; }
    virtual const Attribute & color(Color) const override { return plainAttribute; }
};

static const PlainHighlighter plainHighlighter;


class AnsiAttribute : public Attribute {
protected:
    const char *escape;

public:
    AnsiAttribute(const char *_escape) :
        escape(_escape)
    {}

    void apply(std::ostream& os) const override {
        os << "\33[" << escape;
    }
};

static const AnsiAttribute ansiNormal("0m");
static const AnsiAttribute ansiBold("1m");
static const AnsiAttribute ansiItalic("3m");
static const AnsiAttribute ansiStrike("9m");
static const AnsiAttribute ansiRed("31m");
static const AnsiAttribute ansiGreen("32m");
static const AnsiAttribute ansiBlue("34m");
static const AnsiAttribute ansiGray("37m");


/**
 * Highlighter for plain-text files which outputs ANSI escape codes. See
 * http://en.wikipedia.org/wiki/ANSI_escape_code for more information
 * concerning ANSI escape codes.
 */
class AnsiHighlighter : public Highlighter {
public:
    AnsiHighlighter(void) {}
    virtual const Attribute & normal(void) const override { return ansiNormal; }
    virtual const Attribute & bold(void) const override { return ansiBold; }
    virtual const Attribute & italic(void) const override {
        /* Italic is not widely supported, or worse, implemented with a reverse */
        if (0)
            return ansiItalic;
        else
            return plainAttribute;
    }
    virtual const Attribute & strike(void) const override { return ansiStrike; }
    virtual const Attribute & color(Color c) const override {
        switch (c) {
        case RED:
            return ansiRed;
        case GREEN:
            return ansiGreen;
        case BLUE:
            return ansiBlue;
        case GRAY:
            return ansiGray;
        default:
            return plainAttribute;
        }
    }
};

static const AnsiHighlighter ansiHighlighter;


#ifdef _WIN32


class WindowsAttribute : public Attribute {
protected:
    WORD wAttributes;

public:
    WindowsAttribute(WORD _wAttributes) :
        wAttributes(_wAttributes)
    {}

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

static const WindowsAttribute winNormal(FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED);
static const WindowsAttribute winBold(FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY);
static const WindowsAttribute winItalic(FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED);
static const WindowsAttribute winStrike(COMMON_LVB_REVERSE_VIDEO | FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED);
static const WindowsAttribute winRed(FOREGROUND_RED | FOREGROUND_INTENSITY);
static const WindowsAttribute winGreen(FOREGROUND_GREEN | FOREGROUND_INTENSITY);
static const WindowsAttribute winBlue(FOREGROUND_BLUE | FOREGROUND_INTENSITY);


/**
 * Highlighter for the Windows Console.
 */
class WindowsHighlighter : public Highlighter {
public:
    virtual const Attribute & normal(void) const { return winNormal; }
    virtual const Attribute & bold(void) const { return winBold; }
    virtual const Attribute & italic(void) const { return winItalic; }
    virtual const Attribute & strike(void) const { return winStrike; }
    virtual const Attribute & color(Color c) const {
        switch (c) {
        case RED:
            return winRed;
        case GREEN:
            return winGreen;
        case BLUE:
            return winBlue;
        default:
            return plainAttribute;
        }
    }
};

static const WindowsHighlighter windowsHighlighter;


static bool
haveAnsi(void)
{
    static bool checked = false;
    static bool result = false;

    if (!checked) {
        // https://conemu.github.io/en/ConEmuEnvironment.html
        // XXX: Didn't quite work for me
        if (0) {
            const char *conEmuANSI = getenv("ConEmuANSI");
            if (conEmuANSI &&
                strcmp(conEmuANSI, "ON") == 0) {
                result = true;
                checked = true;
                return result;
            }
        }

        // Cygwin shell
        if (1) {
            const char *term = getenv("TERM");
            if (term &&
                strcmp(term, "xterm") == 0) {
                result = true;
                checked = true;
                return result;
            }
        }

        // http://wiki.winehq.org/DeveloperFaq#detect-wine
        HMODULE hNtDll = GetModuleHandleA("ntdll");
        if (hNtDll) {
            result = GetProcAddress(hNtDll, "wine_get_version") != NULL;
        }

        checked = true;
    }

    return result;
}


#endif /* _WIN32 */


const Highlighter &
defaultHighlighter(bool color) {
    if (color) {
#ifdef _WIN32
        if (haveAnsi()) {
            return ansiHighlighter;
        } else {
            return windowsHighlighter;
        }
#else
        return ansiHighlighter;
#endif
    } else {
        return plainHighlighter;
    }
}


bool
isAtty(std::ostream &os) {
    int fd;
    if (&os == &std::cout) {
#ifdef _WIN32
        fd = _fileno(stdout);
#else
        fd = STDOUT_FILENO;
#endif
    } else if (&os == &std::cerr) {
#ifdef _WIN32
        fd = _fileno(stderr);
#else
        fd = STDERR_FILENO;
#endif
    } else {
        return false;
    }
#ifdef _WIN32
    return _isatty(fd);
#else
    return isatty(fd);
#endif
}


const Highlighter &
defaultHighlighter(std::ostream & os)
{
    return defaultHighlighter(isAtty(os));
}



} /* namespace highlight */
