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
 * Simple OS abstraction layer.
 */

#ifndef _OS_HPP_
#define _OS_HPP_

#include <assert.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include <vector>

#ifdef _WIN32
#ifndef snprintf
#define snprintf _snprintf
#endif
#ifndef vsnprintf
#define vsnprintf _vsnprintf
#endif
#define PATH_SEP '\\'
#else /* !_WIN32 */
#define PATH_SEP '/'
#endif /* !_WIN32 */

#ifndef PATH_MAX
#define PATH_MAX 1024
#endif

namespace os {

void acquireMutex(void);

void releaseMutex(void);


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
        Buffer::iterator sep = rfind(PATH_SEP);
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

    void truncate(size_t size) {
        assert(size > 0);
        assert(size <= buffer.size());
        assert(buffer[size - 1] == 0);
        buffer.resize(size);
    }

    void truncate(void) {
        truncate(strlen(str()));
    }

    const char *str(void) const {
        assert(buffer[buffer.size() - 1] == 0);
        return &buffer[0];
    }

    void join(const Path & other) {
        size_t len = length();
        if (len > 0 && buffer[len - 1] != PATH_SEP) {
            buffer.insert(buffer.begin() + len, PATH_SEP);
        }
        buffer.insert(buffer.begin() + len, other.buffer.begin(), other.buffer.end() - 1);
    }

};

Path getProcessName();
Path getCurrentDir();

void log(const char *format, ...)
#ifdef __GNUC__
    __attribute__ ((format (printf, 1, 2)))
#endif
;

#if defined _WIN32 || defined __CYGWIN__
  /* We always use .def files on windows for now */
  #if 0
  #define PUBLIC __declspec(dllexport)
  #else
  #define PUBLIC
  #endif
  #define PRIVATE
#else
  #if __GNUC__ >= 4
    #define PUBLIC __attribute__ ((visibility("default")))
    #define PRIVATE __attribute__ ((visibility("hidden")))
  #else
    #define PUBLIC
    #define PRIVATE
  #endif
#endif

/**
 * Get the current time in microseconds from an unknown base.
 */
long long getTime(void);

void abort(void);

void setExceptionCallback(void (*callback)(void));
void resetExceptionCallback(void);

} /* namespace os */

#endif /* _OS_HPP_ */
