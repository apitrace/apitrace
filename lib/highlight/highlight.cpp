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

#define DEFINE_CONSOLEV2_PROPERTIES

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

#ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
#define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
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


#define CSI "\33["

class AnsiAttribute : public Attribute {
protected:
    const char *escape;

public:
    AnsiAttribute(const char *_escape) :
        escape(_escape)
    {}

    void apply(std::ostream& os) const override {
        os << escape;
    }
};

static const AnsiAttribute ansiNormal(CSI "0m");
static const AnsiAttribute ansiBold  (CSI "1m");
static const AnsiAttribute ansiItalic(CSI "3m");
static const AnsiAttribute ansiStrike(CSI "9m");
#ifdef _WIN32
// Brighter colors on Windows to contrast with the background, and match the
// colors from SetConsoleTextAttribute
static const AnsiAttribute ansiRed   (CSI "91m");
static const AnsiAttribute ansiGreen (CSI "92m");
static const AnsiAttribute ansiBlue  (CSI "94m");
static const AnsiAttribute ansiGray  (CSI "37m");
#else
static const AnsiAttribute ansiRed   (CSI "31m");
static const AnsiAttribute ansiGreen (CSI "32m");
static const AnsiAttribute ansiBlue  (CSI "34m");
static const AnsiAttribute ansiGray  (CSI "37m");
#endif


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


#endif /* _WIN32 */


const Highlighter &
defaultHighlighter(bool color) {
    if (color) {
#ifdef _WIN32
        static const Highlighter * highlighter = nullptr;
        if (highlighter == nullptr) {
            // http://wiki.winehq.org/DeveloperFaq#detect-wine
            // XXX: ANSI escape codes used to work, but as of wine 6.0.3 both
            // ANSI and console attributes are unusably broken.
            HMODULE hNtDll = GetModuleHandleA("ntdll");
            if (hNtDll &&
                GetProcAddress(hNtDll, "wine_get_version") != nullptr) {
                highlighter = &plainHighlighter;
                return *highlighter;
            }

            // Cygwin shell
            const char *term = getenv("TERM");
            if (term &&
                strcmp(term, "xterm") == 0) {
                highlighter = &ansiHighlighter;
                return *highlighter;
            }

            highlighter = &windowsHighlighter;

            // Try to set output mode to handle virtual terminal sequences
            // https://msdn.microsoft.com/en-us/library/windows/desktop/mt638032.aspx
            // TODO: Use CONOUT$
            // https://msdn.microsoft.com/en-us/library/windows/desktop/ms682075.aspx
            HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
            if (hOut != INVALID_HANDLE_VALUE) {
                DWORD dwMode = 0;
                if (GetConsoleMode(hOut, &dwMode)) {
                    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
                    if (SetConsoleMode(hOut, dwMode)) {
                        highlighter = &ansiHighlighter;
                    }
                }
            }
        }
        return *highlighter;
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
