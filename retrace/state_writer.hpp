/**************************************************************************
 *
 * Copyright 2015 VMware, Inc
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


#pragma once


#include <stddef.h>
#include <wchar.h>

#include <ostream>
#include <type_traits>
#include <string>


namespace image {
    class Image;
}


/*
 * Abstract base class for writing state.
 */
class StateWriter
{
public:
    virtual ~StateWriter();

    virtual void
    beginObject(void) = 0;

    virtual void
    endObject(void) = 0;

    virtual void
    beginMember(const char * name) = 0;

    inline void
    beginMember(const std::string &name) {
        beginMember(name.c_str());
    }

    virtual void
    endMember(void) = 0;

    virtual void
    beginArray(void) = 0;

    virtual void
    endArray(void) = 0;

    virtual void
    writeString(const char *) = 0;

    inline void
    writeString(const std::string &s) {
        writeString(s.c_str());
    }

    virtual void
    writeBlob(const void *bytes, size_t size) = 0;

    virtual void
    writeNull(void) = 0;

    virtual void
    writeBool(bool) = 0;

    // FIXME: always using long long is inneficient,
    // but providing overloaded methods for different size types requires extreme care to avoid ambigiuity

    virtual void
    writeSInt(signed long long) = 0;

    virtual void
    writeUInt(unsigned long long) = 0;

    template<typename T>
    inline void
    writeInt(T i, typename std::enable_if<std::is_signed<T>::value>::type* = 0) {
        writeSInt(i);
    }

    template<typename T>
    inline void
    writeInt(T u, typename std::enable_if<std::is_unsigned<T>::value>::type* = 0) {
        writeUInt(u);
    }

    template<typename T>
    inline void
    writeInt(T e, typename std::enable_if<std::is_enum<T>::value>::type* = 0) {
        writeSInt(static_cast<signed long long>(e));
    }

    virtual void
    writeFloat(float) = 0;

    virtual void
    writeFloat(double) = 0;

    inline void
    writeStringMember(const char *name, const char *s) {
        beginMember(name);
        writeString(s);
        endMember();
    }

    inline void
    writeBoolMember(const char *name, bool b) {
        beginMember(name);
        writeBool(b);
        endMember();
    }

    template<typename T>
    inline void
    writeFloatMember(const char *name, T f) {
        beginMember(name);
        writeFloat(f);
        endMember();
    }

    template<typename T>
    inline void
    writeIntMember(const char *name, T n) {
        beginMember(name);
        writeInt(n);
        endMember();
    }

    struct ImageDesc {
        unsigned depth;
        std::string format;

        ImageDesc() :
            depth(1),
            format("UNKNOWN")
        {}
    };

    void
    writeImage(image::Image *image, const ImageDesc & desc);

    inline void
    writeImage(image::Image *image) {
        ImageDesc desc;
        writeImage(image, desc);
    }

};


StateWriter *
createJSONStateWriter(std::ostream &os);


StateWriter *
createUBJSONStateWriter(std::ostream &os);
