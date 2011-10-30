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
 * Path manipulation.
 */

#ifndef _OS_PATH_HPP_
#define _OS_PATH_HPP_


#include <assert.h>

#include <vector>

#include "os.hpp"


#ifdef _WIN32
#define OS_DIR_SEP '\\'
#else /* !_WIN32 */
#define OS_DIR_SEP '/'
#endif /* !_WIN32 */


namespace os {


class Path {
protected:
    typedef std::vector<char> Buffer;
    Buffer buffer;

    Buffer::iterator rfind(char c) {
        Buffer::iterator it = buffer.end();
        while (it != buffer.begin()) {
            --it;
            if (*it == c) {
                return it;
            }
        }
        return buffer.end();
    }

    Path(size_t size) :
        buffer(size) {
    }

    char *buf(void) {
        return &buffer[0];
    }

public:
    Path() {
        buffer.push_back(0);
    }

    Path(const char *s) :
        buffer(s, s + strlen(s) + 1)
    {}

    template <class InputIterator>
    Path(InputIterator first, InputIterator last) :
        buffer(first, last)
    {
        buffer.push_back(0);
    }

    char *buf(size_t size) {
        buffer.resize(size);
        return &buffer[0];
    }

    void trimDirectory(void) {
        Buffer::iterator sep = rfind(OS_DIR_SEP);
        if (sep != buffer.end()) {
            buffer.erase(buffer.begin(), sep + 1);
        }
    }

    void trimExtension(void) {
        Buffer::iterator dot = rfind('.');
        if (dot != buffer.end()) {
            buffer.erase(dot, buffer.end());
        }
    }

    size_t length(void) const {
        size_t size = buffer.size();
        assert(size > 0);
        assert(buffer[size - 1] == 0);
        return size - 1;
    }

    void truncate(size_t length) {
        assert(length < buffer.size());
        buffer[length] = 0;
        buffer.resize(length + 1);
    }

    void truncate(void) {
        truncate(strlen(str()));
    }

    const char *str(void) const {
        assert(buffer[buffer.size() - 1] == 0);
        return &buffer[0];
    }

    operator const char *(void) const {
        return str();
    }

    void join(const Path & other) {
        size_t len = length();
        if (len > 0 && buffer[len - 1] != OS_DIR_SEP) {
            buffer.insert(buffer.begin() + len++, OS_DIR_SEP);
        }
        buffer.insert(buffer.begin() + len, other.buffer.begin(), other.buffer.end() - 1);
    }

    /**
     * Create a path from a printf-like format string
     */
    static Path
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
#ifdef _MSC_VER
        /* We need to use _vcsprintf to calculate the length as vsnprintf returns -1
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

        Path path(size);

        va_start(args, format);
        vsnprintf(path.buf(), size, format, args);
        va_end(args);

        return path;
    }
};


Path getProcessName();
Path getCurrentDir();


} /* namespace os */

#endif /* _OS_PATH_HPP_ */
