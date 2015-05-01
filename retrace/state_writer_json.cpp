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


#include "state_writer.hpp"

#include "json.hpp"


namespace {


class JSONStateWriter : public StateWriter
{
private:
    JSONWriter json;

public:
    JSONStateWriter(std::ostream &os) :
        json(os)
    {
    }

    void
    beginObject(void) {
        json.beginObject();
    }

    void
    endObject(void) {
        json.endObject();
    }

    void
    beginMember(const char * name) {
        json.beginMember(name);
    }

    void
    endMember(void) {
        json.endMember();
    }

    void
    beginArray(void) {
        json.beginArray();
    }

    void
    endArray(void) {
        json.endArray();
    }

    void
    writeString(const char *s) {
        json.writeString(s);
    }

    void
    writeBlob(const void *bytes, size_t size) {
        json.writeBase64(bytes, size);
    }

    void
    writeNull(void) {
        json.writeNull();
    }

    void
    writeBool(bool b) {
        json.writeBool(b);
    }

    void
    writeSInt(signed long long i) {
        json.writeInt(i);
    }

    void
    writeUInt(unsigned long long u) {
        json.writeInt(u);
    }

    void
    writeFloat(float f) {
        json.writeFloat(f);
    }

    void
    writeFloat(double f) {
        json.writeFloat(f);
    }
};


}


StateWriter *
createJSONStateWriter(std::ostream &os)
{
    return new JSONStateWriter(os);
}
