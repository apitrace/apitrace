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
    beginObject(void) override {
        json.beginObject();
    }

    void
    endObject(void) override {
        json.endObject();
    }

    void
    beginMember(const char * name) override {
        json.beginMember(name);
    }

    void
    endMember(void) override {
        json.endMember();
    }

    void
    beginArray(void) override {
        json.beginArray();
    }

    void
    endArray(void) override {
        json.endArray();
    }

    void
    writeString(const char *s) override {
        json.writeString(s);
    }

    void
    writeBlob(const void *bytes, size_t size) override {
        json.writeBase64(bytes, size);
    }

    void
    writeNull(void) override {
        json.writeNull();
    }

    void
    writeBool(bool b) override {
        json.writeBool(b);
    }

    void
    writeSInt(signed long long i) override {
        json.writeInt(i);
    }

    void
    writeUInt(unsigned long long u) override {
        json.writeInt(u);
    }

    void
    writeFloat(float f) override {
        json.writeFloat(f);
    }

    void
    writeFloat(double f) override {
        json.writeFloat(f);
    }
};


}


StateWriter *
createJSONStateWriter(std::ostream &os)
{
    return new JSONStateWriter(os);
}
