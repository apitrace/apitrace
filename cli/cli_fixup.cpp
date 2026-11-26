/**************************************************************************
 *
 * Copyright 2026 Broadcom
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
 * Audit (and eventually repair) OpenGL traces recorded by older versions of
 * the tracer, whose client memory blobs are smaller than what drivers are
 * allowed to read at replay time:
 *
 * - client vertex arrays whose size was derived from the highest index
 *   instead of the range declared by glDrawRangeElements*, and
 *
 * - pixel uploads whose size didn't account for the row padding implied by
 *   the unpack state.
 *
 * Problems are fixed in place, so the call numbering and the order of the
 * enter/leave events is preserved:
 *
 * - client arrays are zero-padded to the size required by the range; and
 *
 * - tightly packed pixels are respaced as per the unpack state.
 *
 * Every change is logged, showing the call before and after.  Without an
 * output trace this just audits.
 *
 * The checks mirror helpers/glsize.hpp, but work purely off the state
 * recorded in the trace, so no GL implementation is needed.  Enumerants are
 * hardcoded as they are stable.
 *
 * Known limitations: pixel store state is tracked per thread (not per
 * context), and GLES without GL_EXT_unpack_subimage is not distinguished.
 */

#include <stdint.h>
#include <string.h>
#include <getopt.h>

#include <deque>
#include <iostream>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include "cli.hpp"

#include "trace_dump.hpp"
#include "trace_parser.hpp"
#include "trace_writer.hpp"

static const char *synopsis = "Fix client memory blobs that are smaller than what drivers may read.";

static void
usage(void)
{
    std::cout
        << "usage: apitrace fixup [OPTIONS] TRACE_FILE\n"
        << synopsis << "\n"
        "\n"
        "    -h, --help               Show detailed help for fixup options and exit\n"
        "    -o, --output=TRACE_FILE  Output trace file (omit to only audit)\n"
        "    -q, --quiet              Don't log every change, only the summary\n"
    ;
}

const static char *
shortOptions = "ho:q";

const static struct option
longOptions[] = {
    {"help", no_argument, 0, 'h'},
    {"output", required_argument, 0, 'o'},
    {"quiet", no_argument, 0, 'q'},
    {0, 0, 0, 0}
};


namespace {

// Enumerants (stable values from the GL headers)
enum {
    GL_BYTE = 0x1400,
    GL_UNSIGNED_BYTE = 0x1401,
    GL_SHORT = 0x1402,
    GL_UNSIGNED_SHORT = 0x1403,
    GL_INT = 0x1404,
    GL_UNSIGNED_INT = 0x1405,
    GL_FLOAT = 0x1406,
    GL_2_BYTES = 0x1407,
    GL_3_BYTES = 0x1408,
    GL_4_BYTES = 0x1409,
    GL_DOUBLE = 0x140A,
    GL_HALF_FLOAT = 0x140B,
    GL_FIXED = 0x140C,
    GL_BITMAP = 0x1A00,
    GL_HALF_FLOAT_OES = 0x8D61,
    GL_BOOL = 0x8B56,

    GL_UNSIGNED_BYTE_3_3_2 = 0x8032,
    GL_UNSIGNED_SHORT_4_4_4_4 = 0x8033,
    GL_UNSIGNED_SHORT_5_5_5_1 = 0x8034,
    GL_UNSIGNED_INT_8_8_8_8 = 0x8035,
    GL_UNSIGNED_INT_10_10_10_2 = 0x8036,
    GL_UNSIGNED_BYTE_2_3_3_REV = 0x8362,
    GL_UNSIGNED_SHORT_5_6_5 = 0x8363,
    GL_UNSIGNED_SHORT_5_6_5_REV = 0x8364,
    GL_UNSIGNED_SHORT_4_4_4_4_REV = 0x8365,
    GL_UNSIGNED_SHORT_1_5_5_5_REV = 0x8366,
    GL_UNSIGNED_INT_8_8_8_8_REV = 0x8367,
    GL_UNSIGNED_INT_2_10_10_10_REV = 0x8368,
    GL_UNSIGNED_SHORT_8_8_MESA = 0x85BA,
    GL_UNSIGNED_SHORT_8_8_REV_MESA = 0x85BB,
    GL_UNSIGNED_INT_24_8 = 0x84FA,
    GL_UNSIGNED_INT_10F_11F_11F_REV = 0x8C3B,
    GL_UNSIGNED_INT_5_9_9_9_REV = 0x8C3E,
    GL_FLOAT_32_UNSIGNED_INT_24_8_REV = 0x8DAD,
    GL_INT_2_10_10_10_REV = 0x8D9F,
    GL_UNSIGNED_INT_10_10_10_2_OES = 0x8DF6,
    GL_INT_10_10_10_2_OES = 0x8DF7,

    GL_ABGR_EXT = 0x8000,
    GL_INTENSITY = 0x8049,
    GL_COLOR_INDEX = 0x1900,
    GL_STENCIL_INDEX = 0x1901,
    GL_DEPTH_COMPONENT = 0x1902,
    GL_RED = 0x1903,
    GL_GREEN = 0x1904,
    GL_BLUE = 0x1905,
    GL_ALPHA = 0x1906,
    GL_RGB = 0x1907,
    GL_RGBA = 0x1908,
    GL_LUMINANCE = 0x1909,
    GL_LUMINANCE_ALPHA = 0x190A,
    GL_BGR = 0x80E0,
    GL_BGRA = 0x80E1,
    GL_RG = 0x8227,
    GL_RG_INTEGER = 0x8228,
    GL_DEPTH_STENCIL = 0x84F9,
    GL_RED_INTEGER = 0x8D94,
    GL_GREEN_INTEGER = 0x8D95,
    GL_BLUE_INTEGER = 0x8D96,
    GL_RGB_INTEGER = 0x8D98,
    GL_RGBA_INTEGER = 0x8D99,
    GL_BGR_INTEGER = 0x8D9A,
    GL_BGRA_INTEGER = 0x8D9B,

    GL_UNPACK_ROW_LENGTH = 0x0CF2,
    GL_UNPACK_SKIP_ROWS = 0x0CF3,
    GL_UNPACK_SKIP_PIXELS = 0x0CF4,
    GL_UNPACK_ALIGNMENT = 0x0CF5,
    GL_UNPACK_SKIP_IMAGES = 0x806B,
    GL_UNPACK_IMAGE_HEIGHT = 0x806E,

    GL_CLIENT_PIXEL_STORE_BIT = 0x00000001,
};


struct PixelStore {
    int64_t alignment = 4;
    int64_t rowLength = 0;
    int64_t imageHeight = 0;
    int64_t skipRows = 0;
    int64_t skipPixels = 0;
    int64_t skipImages = 0;
};


// Pointer to client memory seen since the previous draw call.
struct PendingArray {
    trace::Call *call;
    unsigned argIndex;
    unsigned callNo;
    std::string name;
    int64_t size;
    int64_t type;
    int64_t stride;
    uint64_t blobSize;
};


struct ThreadState {
    PixelStore pixelStore;
    std::vector<PixelStore> pixelStoreStack;
    std::vector<PendingArray> pending;
};


struct Stats {
    unsigned drawRangeCalls = 0;
    unsigned drawRangeProblems = 0;
    unsigned pixelCalls = 0;
    unsigned pixelProblems = 0;
    unsigned pixelUnsupported = 0;
    unsigned pixelUnfixable = 0;
    unsigned long long fixedBytes = 0;
};


// Returns the size in bytes of a vertex array element, or 0 if unknown.
size_t
typeSize(int64_t type, int64_t size)
{
    switch (type) {
    case GL_BOOL:
    case GL_BYTE:
    case GL_UNSIGNED_BYTE:
        return size * 1;
    case GL_SHORT:
    case GL_UNSIGNED_SHORT:
    case GL_2_BYTES:
    case GL_HALF_FLOAT:
    case GL_HALF_FLOAT_OES:
        return size * 2;
    case GL_3_BYTES:
        return size * 3;
    case GL_INT:
    case GL_UNSIGNED_INT:
    case GL_FLOAT:
    case GL_4_BYTES:
    case GL_FIXED:
        return size * 4;
    case GL_DOUBLE:
        return size * 8;
    case GL_INT_2_10_10_10_REV:
    case GL_INT_10_10_10_2_OES:
    case GL_UNSIGNED_INT_2_10_10_10_REV:
    case GL_UNSIGNED_INT_10_10_10_2_OES:
    case GL_UNSIGNED_INT_10F_11F_11F_REV:
        return 4;
    default:
        return 0;
    }
}


// Mirrors _glArrayPointer_size()
size_t
arrayPointerSize(int64_t size, int64_t type, int64_t stride, int64_t count)
{
    if (count <= 0) {
        return 0;
    }
    if (size == GL_BGRA) {
        size = 4;
    }
    size_t elementSize = typeSize(type, size);
    if (!stride) {
        stride = elementSize;
    }
    return stride * (count - 1) + elementSize;
}


unsigned
formatChannels(int64_t format)
{
    switch (format) {
    case GL_COLOR_INDEX:
    case GL_RED:
    case GL_RED_INTEGER:
    case GL_GREEN:
    case GL_GREEN_INTEGER:
    case GL_BLUE:
    case GL_BLUE_INTEGER:
    case GL_ALPHA:
    case GL_INTENSITY:
    case GL_LUMINANCE:
    case GL_DEPTH_COMPONENT:
    case GL_STENCIL_INDEX:
        return 1;
    case GL_DEPTH_STENCIL:
    case GL_LUMINANCE_ALPHA:
    case GL_RG:
    case GL_RG_INTEGER:
        return 2;
    case GL_RGB:
    case GL_RGB_INTEGER:
    case GL_BGR:
    case GL_BGR_INTEGER:
        return 3;
    case GL_RGBA:
    case GL_RGBA_INTEGER:
    case GL_BGRA:
    case GL_BGRA_INTEGER:
    case GL_ABGR_EXT:
        return 4;
    default:
        return 0;
    }
}


// Mirrors _gl_format_size(); returns 0 if unknown.
unsigned
formatBits(int64_t format, int64_t type)
{
    unsigned n = formatChannels(format);

    switch (type) {
    case GL_BITMAP:
        return 1;
    case GL_BYTE:
    case GL_UNSIGNED_BYTE:
        return 8 * n;
    case GL_SHORT:
    case GL_UNSIGNED_SHORT:
    case GL_HALF_FLOAT:
    case GL_HALF_FLOAT_OES:
        return 16 * n;
    case GL_INT:
    case GL_UNSIGNED_INT:
    case GL_FLOAT:
        return 32 * n;
    case GL_UNSIGNED_BYTE_3_3_2:
    case GL_UNSIGNED_BYTE_2_3_3_REV:
        return 8;
    case GL_UNSIGNED_SHORT_4_4_4_4:
    case GL_UNSIGNED_SHORT_4_4_4_4_REV:
    case GL_UNSIGNED_SHORT_5_5_5_1:
    case GL_UNSIGNED_SHORT_1_5_5_5_REV:
    case GL_UNSIGNED_SHORT_5_6_5:
    case GL_UNSIGNED_SHORT_5_6_5_REV:
    case GL_UNSIGNED_SHORT_8_8_MESA:
    case GL_UNSIGNED_SHORT_8_8_REV_MESA:
        return 16;
    case GL_UNSIGNED_INT_8_8_8_8:
    case GL_UNSIGNED_INT_8_8_8_8_REV:
    case GL_UNSIGNED_INT_10_10_10_2:
    case GL_UNSIGNED_INT_2_10_10_10_REV:
    case GL_UNSIGNED_INT_24_8:
    case GL_UNSIGNED_INT_10F_11F_11F_REV:
    case GL_UNSIGNED_INT_5_9_9_9_REV:
        return 32;
    case GL_FLOAT_32_UNSIGNED_INT_24_8_REV:
        return 64;
    default:
        return 0;
    }
}


// Mirrors _gl_image_size(); returns 0 if the format/type is unknown.
uint64_t
imageSize(const PixelStore &ps, int64_t format, int64_t type,
          int64_t width, int64_t height, int64_t depth)
{
    uint64_t bitsPerPixel = formatBits(format, type);
    if (!bitsPerPixel) {
        return 0;
    }

    int64_t rowLength = ps.rowLength > 0 ? ps.rowLength : width;
    uint64_t rowStride = (rowLength * bitsPerPixel + 7) / 8;

    int64_t alignment = ps.alignment;
    if (alignment > 0 && (alignment & (alignment - 1)) == 0) {
        rowStride = (rowStride + (alignment - 1)) & ~(uint64_t)(alignment - 1);
    }

    int64_t imageHeight = ps.imageHeight > 0 ? ps.imageHeight : height;
    uint64_t imageStride = imageHeight * rowStride;

    uint64_t size = (width * bitsPerPixel + 7) / 8;
    if (height > 1) {
        size += (height - 1) * rowStride;
    }
    if (depth > 1) {
        size += (depth - 1) * imageStride;
    }

    size += (ps.skipPixels * bitsPerPixel + 7) / 8;
    size += ps.skipRows * rowStride;
    size += ps.skipImages * imageStride;

    return size;
}


bool
startsWith(const char *s, const char *prefix)
{
    return strncmp(s, prefix, strlen(prefix)) == 0;
}

bool
endsWith(const char *s, const char *suffix)
{
    size_t n = strlen(s), m = strlen(suffix);
    return n >= m && strcmp(s + n - m, suffix) == 0;
}


// Returns the vertex array pointer size/type defaults for a gl*Pointer call,
// or false if this isn't one.
bool
isArrayPointer(const char *name)
{
    if (!startsWith(name, "gl")) {
        return false;
    }
    // Skip the (EXT) variants, which carry an explicit count.
    return endsWith(name, "Pointer") ||
           endsWith(name, "PointerARB") ||
           endsWith(name, "PointerNV");
}


bool
isDraw(const char *name)
{
    return startsWith(name, "glDraw") || startsWith(name, "glMultiDraw");
}


bool
isDrawRangeElements(const char *name)
{
    return strcmp(name, "glDrawRangeElements") == 0 ||
           strcmp(name, "glDrawRangeElementsEXT") == 0 ||
           startsWith(name, "glDrawRangeElementsBaseVertex");
}


// Rearranges tightly packed pixels (as if GL_UNPACK_ALIGNMENT was 1) into the
// layout given by the pixel store state, zero-filling the padding.
trace::Blob *
respace(const trace::Blob *blob, const PixelStore &ps, int64_t format, int64_t type,
        int64_t width, int64_t height, int64_t depth, uint64_t newSize)
{
    uint64_t bitsPerPixel = formatBits(format, type);
    uint64_t rowBytes = (width * bitsPerPixel + 7) / 8;

    int64_t rowLength = ps.rowLength > 0 ? ps.rowLength : width;
    int64_t imageHeight = ps.imageHeight > 0 ? ps.imageHeight : height;

    uint64_t srcRowStride = (rowLength * bitsPerPixel + 7) / 8;
    uint64_t dstRowStride = srcRowStride;
    if (ps.alignment > 0 && (ps.alignment & (ps.alignment - 1)) == 0) {
        dstRowStride = (dstRowStride + (ps.alignment - 1)) & ~(uint64_t)(ps.alignment - 1);
    }
    uint64_t srcImageStride = imageHeight * srcRowStride;
    uint64_t dstImageStride = imageHeight * dstRowStride;

    uint64_t skipBytes = (ps.skipPixels * bitsPerPixel + 7) / 8;
    uint64_t srcSkip = skipBytes + ps.skipRows * srcRowStride + ps.skipImages * srcImageStride;
    uint64_t dstSkip = skipBytes + ps.skipRows * dstRowStride + ps.skipImages * dstImageStride;

    trace::Blob *result = new trace::Blob(newSize);
    memset(result->buf, 0, newSize);

    for (int64_t z = 0; z < depth; ++z) {
        for (int64_t y = 0; y < height; ++y) {
            uint64_t src = srcSkip + z * srcImageStride + y * srcRowStride;
            uint64_t dst = dstSkip + z * dstImageStride + y * dstRowStride;
            if (src + rowBytes <= blob->size && dst + rowBytes <= newSize) {
                memcpy(result->buf + dst, blob->buf + src, rowBytes);
            }
        }
    }

    return result;
}


std::string
callToString(trace::Call &call)
{
    std::ostringstream os;
    trace::dump(call, os, trace::DUMP_FLAG_NO_COLOR | trace::DUMP_FLAG_NO_MULTILINE);
    return os.str();
}


// Parses the trace events, fixing and writing them out in the original order.
class Fixer : public trace::SimpleParser {
public:
    Fixer(bool _quiet, trace::Writer *_writer) : quiet(_quiet), writer(_writer) {}

    ~Fixer();

    // Flush all delayed events.
    void finish(void);

    void summary(std::ostream &os) const;

protected:
    // Events are written possibly delayed, as a client array pointer call can
    // only be fixed once the draw call that uses it is seen.
    void handleEnter(trace::Call *call) override;
    void handleLeave(trace::Call *call) override;

private:
    // Safety valve for pointer calls that are never followed by a draw call.
    static const size_t maxDelayed = 100000;

    bool quiet;
    trace::Writer *writer;
    std::map<unsigned, ThreadState> threads;

    // Events not written yet.  Each event's call is deleted once written.
    enum EventType {
        ENTER,
        LEAVE,
    };
    struct Event {
        EventType type;
        trace::Call *call;
    };
    std::deque<Event> delayed;

    // Number given by the writer to the calls entered but not left yet, by
    // their original number.
    std::map<unsigned, unsigned> written;

    Stats stats;

    void onPixelStorei(ThreadState &ts, trace::Call *call);
    void onArrayPointer(ThreadState &ts, trace::Call *call);
    void onDrawRangeElements(ThreadState &ts, trace::Call *call);
    void onPixelUpload(ThreadState &ts, trace::Call *call);

    bool anyPending(void) const;
    void flush(void);
    void flushIfPossible(void);

    void logChange(const std::string &what, const std::string &before, trace::Call *after);
};


void
Fixer::logChange(const std::string &what, const std::string &before, trace::Call *after)
{
    if (quiet) {
        return;
    }
    std::cout
        << what << "\n"
        << "  before: " << before << "\n"
        << "  after:  " << callToString(*after) << "\n";
}


void
Fixer::onPixelStorei(ThreadState &ts, trace::Call *call)
{
    int64_t pname = call->arg(0).toSInt();
    int64_t param = call->arg(1).toSInt();
    switch (pname) {
    case GL_UNPACK_ALIGNMENT:   ts.pixelStore.alignment = param; break;
    case GL_UNPACK_ROW_LENGTH:  ts.pixelStore.rowLength = param; break;
    case GL_UNPACK_IMAGE_HEIGHT: ts.pixelStore.imageHeight = param; break;
    case GL_UNPACK_SKIP_ROWS:   ts.pixelStore.skipRows = param; break;
    case GL_UNPACK_SKIP_PIXELS: ts.pixelStore.skipPixels = param; break;
    case GL_UNPACK_SKIP_IMAGES: ts.pixelStore.skipImages = param; break;
    default: break;
    }
}


void
Fixer::onArrayPointer(ThreadState &ts, trace::Call *call)
{
    const char *name = call->sig->name;

    PendingArray a;
    a.call = call;
    a.argIndex = ~0u;
    a.callNo = call->no;
    a.name = name;

    // Arguments absent from the signature take these implicit values.
    a.size = 1;
    a.type = GL_FLOAT;
    a.stride = 0;
    if (startsWith(name, "glNormalPointer")) {
        a.size = 3;
    } else if (startsWith(name, "glEdgeFlagPointer")) {
        a.type = GL_BOOL;
    }

    for (unsigned i = 0; i < call->sig->num_args; ++i) {
        const char *argName = call->sig->arg_names[i];
        if (strcmp(argName, "size") == 0) {
            a.size = call->arg(i).toSInt();
        } else if (strcmp(argName, "type") == 0) {
            a.type = call->arg(i).toSInt();
        } else if (strcmp(argName, "stride") == 0) {
            a.stride = call->arg(i).toSInt();
        } else if (strcmp(argName, "pointer") == 0) {
            a.argIndex = i;
        }
    }

    if (a.argIndex == ~0u) {
        return;
    }

    const trace::Blob *blob = call->arg(a.argIndex).toBlob();
    if (!blob) {
        // Buffer object offset or NULL
        return;
    }
    a.blobSize = blob->size;

    ts.pending.push_back(a);
}


void
Fixer::onDrawRangeElements(ThreadState &ts, trace::Call *call)
{
    uint64_t end = call->argByName("end").toUInt() & 0xffffffffu;
    int64_t baseVertex = call->argByName("basevertex").toSInt();

    // The tracer falls back to scanning the indices in this case.
    if (end == 0xffffffffu) {
        return;
    }

    int64_t count = (int64_t)end + baseVertex + 1;

    ++stats.drawRangeCalls;

    for (const PendingArray &a : ts.pending) {
        uint64_t required = arrayPointerSize(a.size, a.type, a.stride, count);
        if (!required || a.blobSize >= required) {
            continue;
        }

        std::string before = callToString(*a.call);

        const trace::Blob *old = a.call->arg(a.argIndex).toBlob();
        trace::Blob *padded = new trace::Blob(required);
        memcpy(padded->buf, old->buf, old->size);
        memset(padded->buf + old->size, 0, required - old->size);
        delete a.call->args[a.argIndex].value;
        a.call->args[a.argIndex].value = padded;

        ++stats.drawRangeProblems;
        stats.fixedBytes += required - a.blobSize;

        std::ostringstream what;
        what << a.callNo << " " << a.name << ": client array padded from "
             << a.blobSize << " to " << required << " bytes, as required by call "
             << call->no << " " << call->sig->name << " (end=" << end
             << " basevertex=" << baseVertex << ")";
        logChange(what.str(), before, a.call);
    }
}


void
Fixer::onPixelUpload(ThreadState &ts, trace::Call *call)
{
    const char *name = call->sig->name;
    bool isBitmap = strcmp(name, "glBitmap") == 0;

    int dims;
    if (isBitmap || strcmp(name, "glDrawPixels") == 0) {
        dims = 2;
    } else {
        const char *p = strstr(name, "Image");
        dims = p ? p[5] - '0' : 0;
        if (dims < 1 || dims > 3) {
            return;
        }
    }

    const char *pixelsName = isBitmap ? "bitmap" : "pixels";
    unsigned argIndex = ~0u;
    for (unsigned i = 0; i < call->sig->num_args; ++i) {
        if (strcmp(call->sig->arg_names[i], pixelsName) == 0) {
            argIndex = i;
        }
    }
    if (argIndex == ~0u) {
        return;
    }

    const trace::Blob *blob = call->arg(argIndex).toBlob();
    if (!blob) {
        // Pixel buffer object offset or NULL
        return;
    }

    int64_t width = call->argByName("width").toSInt();
    int64_t height = dims >= 2 ? call->argByName("height").toSInt() : 1;
    int64_t depth = dims >= 3 ? call->argByName("depth").toSInt() : 1;
    int64_t format, type;
    if (isBitmap) {
        format = GL_COLOR_INDEX;
        type = GL_BITMAP;
    } else {
        format = call->argByName("format").toSInt();
        type = call->argByName("type").toSInt();
    }

    ++stats.pixelCalls;

    const PixelStore &ps = ts.pixelStore;

    uint64_t required = imageSize(ps, format, type, width, height, depth);
    if (!required) {
        ++stats.pixelUnsupported;
        if (!quiet) {
            std::cout << call->no << " " << name << ": unsupported format 0x"
                      << std::hex << format << " / type 0x" << type << std::dec
                      << ", not checked\n";
        }
        return;
    }

    if (blob->size >= required) {
        return;
    }

    std::ostringstream state;
    state << "alignment=" << ps.alignment << " row_length=" << ps.rowLength
          << " image_height=" << ps.imageHeight
          << " skip_rows=" << ps.skipRows << " skip_pixels=" << ps.skipPixels
          << " skip_images=" << ps.skipImages;

    // Only blobs that are exactly the tightly packed image can be respaced.
    PixelStore tight = ps;
    tight.alignment = 1;
    if (blob->size != imageSize(tight, format, type, width, height, depth)) {
        ++stats.pixelUnfixable;
        std::cout << call->no << " " << name << ": pixels are " << blob->size
                  << " bytes, but " << required << " are required, and that "
                  << "isn't a tightly packed layout: NOT FIXED ("
                  << state.str() << ")\n";
        return;
    }

    std::string before = callToString(*call);

    uint64_t oldSize = blob->size;
    trace::Blob *spaced = respace(blob, ps, format, type, width, height, depth, required);
    delete call->args[argIndex].value;
    call->args[argIndex].value = spaced;

    ++stats.pixelProblems;
    stats.fixedBytes += required - oldSize;

    std::ostringstream what;
    what << call->no << " " << name << ": pixel rows respaced from tightly packed ("
         << oldSize << " bytes) to " << required << " bytes, as per the unpack state ("
         << state.str() << ")";
    logChange(what.str(), before, call);
}


bool
Fixer::anyPending(void) const
{
    for (const auto &it : threads) {
        if (!it.second.pending.empty()) {
            return true;
        }
    }
    return false;
}


void
Fixer::flush(void)
{
    for (const Event &event : delayed) {
        trace::Call *call = event.call;
        switch (event.type) {
        case ENTER:
            if (writer) {
                written[call->no] = writer->writeEnter(call);
            }
            break;
        case LEAVE:
            if (writer) {
                auto it = written.find(call->no);
                if (it != written.end()) {
                    writer->writeLeave(call, it->second);
                    written.erase(it);
                }
            }
            break;
        }
        delete call;
    }
    delayed.clear();
}


Fixer::~Fixer()
{
    for (const Event &event : delayed) {
        delete event.call;
    }
}


void
Fixer::finish(void)
{
    // Drop references to the calls about to be flushed.
    for (auto &it : threads) {
        it.second.pending.clear();
    }
    flush();
}


void
Fixer::handleEnter(trace::Call *call)
{
    if (call->flags & trace::CALL_FLAG_INCOMPLETE) {
        // Truncated at the end of the file
        delete call;
        return;
    }

    const char *name = call->sig->name;
    ThreadState &ts = threads[call->thread_id];

    if (strcmp(name, "glPixelStorei") == 0) {
        onPixelStorei(ts, call);
    } else if (strcmp(name, "glPushClientAttrib") == 0 ||
               strcmp(name, "glPushClientAttribDefaultEXT") == 0) {
        ts.pixelStoreStack.push_back(ts.pixelStore);
    } else if (strcmp(name, "glPopClientAttrib") == 0) {
        if (!ts.pixelStoreStack.empty()) {
            // We don't track the masks, so assume pixel store is always saved
            // (and restored): the most common usage by far.
            ts.pixelStore = ts.pixelStoreStack.back();
            ts.pixelStoreStack.pop_back();
        }
    } else if (isArrayPointer(name)) {
        onArrayPointer(ts, call);
    } else if (isDraw(name)) {
        if (isDrawRangeElements(name)) {
            onDrawRangeElements(ts, call);
        }
        ts.pending.clear();
    } else if (strcmp(name, "glBitmap") == 0 ||
               strcmp(name, "glDrawPixels") == 0 ||
               (startsWith(name, "gl") && !startsWith(name, "glGet") &&
                !startsWith(name, "glRead") &&
                (strstr(name, "TexImage") || strstr(name, "TexSubImage") ||
                 strstr(name, "TextureImage") || strstr(name, "TextureSubImage")))) {
        onPixelUpload(ts, call);
    }

    delayed.push_back({ENTER, call});
    flushIfPossible();
}


void
Fixer::handleLeave(trace::Call *call)
{
    if (call->flags & trace::CALL_FLAG_INCOMPLETE) {
        // Truncated at the end of the file
        delete call;
        return;
    }

    delayed.push_back({LEAVE, call});
    flushIfPossible();
}


void
Fixer::flushIfPossible(void)
{
    if (!anyPending()) {
        flush();
    } else if (delayed.size() > maxDelayed) {
        std::cerr << "warning: no draw call after client array pointer call(s); "
                     "giving up on fixing them\n";
        for (auto &it : threads) {
            it.second.pending.clear();
        }
        flush();
    }
}


void
Fixer::summary(std::ostream &os) const
{
    os << "Client arrays padded: " << stats.drawRangeProblems
       << " (out of " << stats.drawRangeCalls << " glDrawRangeElements* calls with a range)\n";
    os << "Pixel uploads respaced: " << stats.pixelProblems
       << " (out of " << stats.pixelCalls << " client memory uploads)\n";
    if (stats.pixelUnfixable) {
        os << "Pixel uploads NOT fixed: " << stats.pixelUnfixable << "\n";
    }
    if (stats.pixelUnsupported) {
        os << "Pixel uploads not checked (unknown format/type): " << stats.pixelUnsupported << "\n";
    }
    os << "Total bytes added: " << stats.fixedBytes << "\n";
}

} // anonymous namespace


static int
fixup_trace(const char *filename, const std::string &output, bool quiet)
{
    trace::Writer writer;
    bool writing = !output.empty();

    Fixer fixer(quiet, writing ? &writer : nullptr);

    if (!fixer.open(filename)) {
        std::cerr << "error: failed to open " << filename << "\n";
        return 1;
    }

    if (writing) {
        if (!writer.open(output.c_str(), fixer.getVersion(), fixer.getProperties())) {
            std::cerr << "error: failed to create " << output << "\n";
            return 1;
        }
    }

    while (fixer.parse_event()) {
    }
    fixer.finish();

    fixer.summary(std::cout);

    if (writing) {
        std::cerr << "Output trace is available as " << output << "\n";
    }

    return 0;
}


static int
command(int argc, char *argv[])
{
    std::string output;
    bool quiet = false;

    int opt;
    while ((opt = getopt_long(argc, argv, shortOptions, longOptions, NULL)) != -1) {
        switch (opt) {
        case 'h':
            usage();
            return 0;
        case 'o':
            output = optarg;
            break;
        case 'q':
            quiet = true;
            break;
        default:
            std::cerr << "error: unexpected option `" << (char)opt << "`\n";
            usage();
            return 1;
        }
    }

    if (optind >= argc) {
        std::cerr << "error: apitrace fixup requires a trace file as an argument.\n";
        usage();
        return 1;
    }

    if (argc > optind + 1) {
        std::cerr << "error: extraneous arguments:";
        for (int i = optind + 1; i < argc; i++) {
            std::cerr << " " << argv[i];
        }
        std::cerr << "\n";
        usage();
        return 1;
    }

    return fixup_trace(argv[optind], output, quiet);
}

const Command fixup_command = {
    "fixup",
    synopsis,
    usage,
    command
};
