/**************************************************************************
 *
 * Copyright 2011 Jose Fonseca
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
 * String manipulation.
 */

#ifndef _OS_STRING_HPP_
#define _OS_STRING_HPP_


#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <stddef.h>

#ifdef __MINGW32__
// Some versions of MinGW are missing _vscprintf's declaration, although they
// still provide the symbol in the import library.
extern "C" _CRTIMP int _vscprintf(const char *format, va_list argptr);
#endif

#ifndef va_copy
#ifdef __va_copy
#define va_copy(dest, src) __va_copy((dest), (src))
#else
#define va_copy(dest, src) (dest) = (src)
#endif
#endif

#include <vector>

#include "os.hpp"


#ifdef _WIN32
#define OS_DIR_SEP '\\'
#define OS_PATH_SEP ';'
#else /* !_WIN32 */
#define OS_DIR_SEP '/'
#define OS_PATH_SEP ':'
#endif /* !_WIN32 */


namespace os {


/**
 * Class to represent zero-terminated strings, based upon std::vector<char>,
 * suitable for passing strings or paths to/from OS calls.
 *
 * Both Win32 and POSIX APIs return strings as zero length buffers.  Although
 * std::string provides an easy method to obtain a read-only pointer to a zero
 * terminated string, it lacks the ability to return a read-write pointer. So
 * there is no way to tell OS calls to write into a std::string directly -- a
 * temporary malloc'ed string would be necessary --, which would be
 * unnecessarily inefficient, specially considering that these strings would
 * ultimately passed back to the OS, which would again expect zero-terminated
 * strings.
 *
 * This class is not, however, a full replacement for std::string, which should
 * be otherwise used whenever possible.
 */
class String {
protected:
    typedef std::vector<char> Buffer;

    /**
     * The buffer's last element is always the '\0' character, therefore the
     * buffer must never be empty.
     */
    Buffer buffer;

    Buffer::iterator find(char c) {
        Buffer::iterator it = buffer.begin();
        assert(it != buffer.end());
        while (it != buffer.end()) {
            if (*it == c) {
                return it;
            }
            ++it;
        }
        return buffer.end();
    }

    Buffer::iterator rfind(char c) {
        Buffer::iterator it = buffer.end();

        // Skip trailing '\0'
        assert(it != buffer.begin());
        --it;
        assert(*it == '\0');

        while (it != buffer.begin()) {
            --it;
            if (*it == c) {
                return it;
            }
        }

        return buffer.end();
    }

    String(size_t size) :
        buffer(size) {
    }

    char *buf(void) {
        return &buffer[0];
    }

    inline bool
    isSep(char c) {
        if (c == '/') {
            return true;
        }
#ifdef _WIN32
        if (c == '\\') {
            return true;
        }
#endif
        return false;
    }

public:

    Buffer::iterator rfindSep(bool skipTrailing = true) {
        Buffer::iterator it = end();

        // Skip trailing separators
        if (skipTrailing) {
            while (it != buffer.begin()) {
                --it;
                if (isSep(*it)) {
                    // Halt if find the root
                    if (it == buffer.begin()) {
                        return it;
                    }
                } else {
                    break;
                }
            }
        }

        // Advance to the last separator
        while (it != buffer.begin()) {
            --it;
            if (isSep(*it)) {
                return it;
            }
        }

        return end();
    }

    /*
     * Constructors
     */

    String() {
        buffer.push_back(0);
    }

    String(const char *s) :
        buffer(s, s + strlen(s) + 1)
    {}

    String(const String &other) :
        buffer(other.buffer)
    {}

    template <class InputIterator>
    String(InputIterator first, InputIterator last) :
        buffer(first, last)
    {
        buffer.push_back(0);
    }

    /**
     * From a printf-like format string
     */
    static String
    format(const char *format, ...)
#ifdef __GNUC__
    __attribute__ ((format (printf, 1, 2)))
#endif
    {

        va_list args;

        va_start(args, format);

        int length;
        va_list args_copy;
        va_copy(args_copy, args);
#ifdef _WIN32
        /* We need to use _vscprintf to calculate the length as vsnprintf returns -1
         * if the number of characters to write is greater than count.
         */
        length = _vscprintf(format, args_copy);
#else
        char dummy;
        length = vsnprintf(&dummy, sizeof dummy, format, args_copy);
#endif
        va_end(args_copy);

        assert(length >= 0);
        size_t size = length + 1;

        String path(size);

        va_start(args, format);
        vsnprintf(path.buf(), size, format, args);
        va_end(args);

        return path;
    }

    /*
     * Conversion to ordinary C strings.
     */

    const char *str(void) const {
        assert(buffer.back() == 0);
        return &buffer[0];
    }

    operator const char *(void) const {
        return str();
    }

    /*
     * Iterators
     */

    typedef Buffer::const_iterator const_iterator;
    typedef Buffer::iterator iterator;

    const_iterator begin(void) const {
        return buffer.begin();
    }

    iterator begin(void) {
        return buffer.begin();
    }

    const_iterator end(void) const {
        const_iterator it = buffer.end();
        assert(it != buffer.begin());
        --it; // skip null
        return it;
    }

    iterator end(void) {
        iterator it = buffer.end();
        assert(it != buffer.begin());
        --it; // skip null
        return it;
    }

    /*
     * Operations
     */

    void insert(iterator position, char c) {
        buffer.insert(position, c);
    }

    template <class InputIterator>
    void insert(iterator position, InputIterator first, InputIterator last) {
        buffer.insert(position, first, last);
    }

    void insert(iterator position, const char *s) {
        assert(s);
        insert(position, s, s + strlen(s));
    }

    void insert(iterator position, const String & other) {
        insert(position, other.begin(), other.end());
    }

    void append(char c) {
        insert(end(), c);
    }

    template <class InputIterator>
    void append(InputIterator first, InputIterator last) {
        insert(end(), first, last);
    }

    void append(const char *s) {
        insert(end(), s);
    }

    void append(const String & other) {
        insert(end(), other);
    }

    template <class InputIterator>
    void erase(InputIterator first, InputIterator last) {
        buffer.erase(first, last);
    }

    /**
     * Get a writable buffer with the specified size.
     *
     * truncate() must be called after the buffer is written, and before any other
     * method is called.
     *
     * Between the call to buf() and truncate() methods, the `buffer.back() ==
     * 0` invariant will not hold true.
     */
    char *buf(size_t size) {
        buffer.resize(size);
        return &buffer[0];
    }

    size_t length(void) const {
        size_t size = buffer.size();
        assert(size > 0);
        assert(buffer[size - 1] == 0);
        return size - 1;
    }

    /**
     * Truncate the string to the specified length.
     */
    void truncate(size_t length) {
        assert(length < buffer.size());
        buffer[length] = 0;
        assert(strlen(&buffer[0]) == length);
        buffer.resize(length + 1);
    }

    /**
     * Truncate the string to the first zero character.
     */
    void truncate(void) {
        truncate(strlen(&buffer[0]));
    }


    /*
     * Path manipulation
     */

    bool
    exists(void) const;

    /* Trim directory (leaving base filename).
     */
    void trimDirectory(void) {
        iterator sep = rfindSep();
        if (sep != end()) {
            buffer.erase(buffer.begin(), sep + 1);
        }
    }

    /* Trim filename component (leaving containing directory).
     *
     * - trailing separators are ignored
     * - a path with no separator at all yields "."
     * - a path consisting of just the root directory is left unchanged
     */
    void trimFilename(void) {
        iterator sep = rfindSep();

        // No separator found, so return '.'
        if (sep == end()) {
            buffer.resize(2);
            buffer[0] = '.';
            buffer[1] = 0;
            return;
        }

        // Root. Nothing to do.
        if (sep == buffer.begin()) {
            return;
        }

        // Trim filename
        buffer.erase(sep, end());
    }

    void trimExtension(void) {
        iterator dot = rfind('.');
        if (dot != buffer.end()) {
            buffer.erase(dot, end());
        }
    }

    void join(const String & other) {
        if (length() && end()[-1] != OS_DIR_SEP) {
            append(OS_DIR_SEP);
        }
        append(other.begin(), other.end());
    }
};


String getProcessName();
String getCurrentDir();

bool createDirectory(const String &path);

bool copyFile(const String &srcFileName, const String &dstFileName, bool override = true);

bool removeFile(const String &fileName);

} /* namespace os */

#endif /* _OS_STRING_HPP_ */
