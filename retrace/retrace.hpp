/**************************************************************************
 *
 * Copyright 2011-2012 Jose Fonseca
 * Copyright (C) 2013 Intel Corporation. All rights reversed.
 * Author: Shuang He <shuang.he@intel.com>
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

#include <assert.h>
#include <string.h>

#include <list>
#include <map>
#include <ostream>

#ifdef _WIN32
#include <windows.h>
#endif

#include "trace_model.hpp"
#include "trace_parser.hpp"
#include "trace_profiler.hpp"
#include "trace_dump.hpp"

#include "scoped_allocator.hpp"


namespace image {
    class Image;
}

class StateWriter;


namespace retrace {


extern trace::AbstractParser *parser;
extern trace::Profiler profiler;


class ScopedAllocator : public ::ScopedAllocator
{
public:
    /**
     * Allocate an array with the same dimensions as the specified value.
     */
    inline void *
    allocArray(const trace::Value *value, size_t elemSize) {
        const trace::Array *array = value->toArray();
        if (array) {
            size_t numElems = array->size();
            size_t size = numElems * elemSize;
            void *ptr = ::ScopedAllocator::alloc(size);
            if (ptr) {
                memset(ptr, 0, size);
            }
            return ptr;
        }
        const trace::Null *null = value->toNull();
        if (null) {
            return NULL;
        }
        assert(0);
        return NULL;
    }

    /**
     * XXX: We must not compute sizeof(T) inside the function body! d3d8.h and
     * d3d9.h have declarations of D3DPRESENT_PARAMETERS and D3DVOLUME_DESC
     * structures with different size sizes.  Multiple specializations of these
     * will be produced (on debug builds, as on release builds the whole body
     * is inlined.), and the linker will pick up one, leading to wrong results
     * if the smallest specialization is picked.
     */
    template< class T >
    inline T *
    allocArray(const trace::Value *value, size_t sizeof_T = sizeof(T)) {
        return static_cast<T *>(allocArray(value, sizeof_T));
    }

};


enum EQueryHandling {
    QUERY_SKIP,
    QUERY_RUN,
    QUERY_RUN_AND_CHECK_RESULT,
};

extern EQueryHandling queryHandling;

extern unsigned queryTolerance;

/**
 * Output verbosity when retracing files.
 */
extern int verbosity;

/**
 * Debugging checks.
 */
extern int debug;

/**
 * Call no markers.
 */
extern bool markers;

/**
 * Snapshot all render targets plus depth/stencil.
 */
extern bool snapshotMRT;


/**
 * Whether to include alpha in snapshots.
 */
extern bool snapshotAlpha;

/**
 * Whether to force windowed. Recommeded, as there is no guarantee that the
 * original display mode is available.
 */
extern bool forceWindowed;

/**
 * Whether to ignore the call's recorded return value.  For a call like
 * glx/wglMakeCurrent() it means that if the call failed during recording and
 * returned FALSE, we ignore that at replay time and try to execute the
 * glx/wglMakeCurrent() normally (instead of no-oping it).
 */
extern bool ignoreRetvals;

extern bool contextCheck;

/**
 * Add profiling data to the dump when retracing.
 */
extern unsigned curPass;
extern unsigned numPasses;
extern bool profilingWithBackends;
extern char* profilingCallsMetricsString;
extern char* profilingFramesMetricsString;
extern char* profilingDrawCallsMetricsString;
extern bool profilingListMetrics;
extern bool profilingNumPasses;

extern bool profiling;
extern bool profilingFrameTimes;
extern bool profilingCpuTimes;
extern bool profilingGpuTimes;
extern bool profilingPixelsDrawn;
extern bool profilingMemoryUsage;

/**
 * State dumping.
 */
extern bool dumpingState;
extern bool dumpingSnapshots;


enum Driver {
    DRIVER_DEFAULT,
    DRIVER_HARDWARE, // force hardware
    DRIVER_DISCRETE, // force discrete GPU
    DRIVER_INTEGRATED, // force integrated GPU
    DRIVER_SOFTWARE,
    DRIVER_REFERENCE,
    DRIVER_NULL,
    DRIVER_MODULE,
};

extern Driver driver;
extern const char *driverModule;

extern bool doubleBuffer;
extern unsigned samples;

extern unsigned frameNo;
extern unsigned callNo;

extern trace::DumpFlags dumpFlags;

std::ostream &warning(trace::Call &call);

#ifdef _WIN32
void failed(trace::Call &call, HRESULT hr);
#endif

void checkMismatch(trace::Call &call, const char *expr, trace::Value *traceValue, long actualValue);

void ignore(trace::Call &call);
void unsupported(trace::Call &call);


typedef void (*Callback)(trace::Call &call);

struct Entry {
    const char *name;
    Callback callback;
};


struct stringComparer {
  bool operator() (const char *a, const  char *b) const {
    return strcmp(a, b) < 0;
  }
};


extern const Entry stdc_callbacks[];


class Retracer
{
    typedef std::map<const char *, Callback, stringComparer> Map;
    Map map;

    std::vector<Callback> callbacks;

public:
    Retracer() {
        addCallbacks(stdc_callbacks);
    }

    virtual ~Retracer() {}

    void addCallback(const Entry *entry);
    void addCallbacks(const Entry *entries);

    void retrace(trace::Call &call);
};


class Dumper
{
public:
    virtual int
    getSnapshotCount(void) = 0;

    virtual image::Image *
    getSnapshot(int n, bool backBuffer) = 0;

    virtual bool
    canDump(void) = 0;

    virtual void
    dumpState(StateWriter &) = 0;
};


extern Dumper *dumper;


void
setFeatureLevel(const char *featureLevel);

void
setUp(void);

void
addCallbacks(retrace::Retracer &retracer);

void
frameComplete(trace::Call &call);


/**
 * Flush rendering (called when switching threads).
 */
void
flushRendering(void);

/**
 * Finish rendering (called before exiting.)
 */
void
finishRendering(void);

void
waitForInput(void);

void
cleanUp(void);

enum class ValueTypeKind {
    _void,
    literal,
    _const,
    pointer,
    int_pointer,
    obj_pointer,
    linear_pointer,
    reference,
    handle,
    _enum,
    bitmask,
    array,
    attrib_array,
    blob,
    _struct,
    alias,
    string,
    opaque,
    polymorphic,
};

class ValueType {
public:
    ValueTypeKind kind;
    std::string c_decl;

    ValueType(ValueTypeKind kind, const std::string &c_decl) : kind(kind), c_decl(c_decl) {}
};

class VoidType : public ValueType {
public:
    VoidType() : ValueType(ValueTypeKind::_void, "void") {}
};

class LiteralType : public ValueType {
public:
    const char *encodedKind;

    LiteralType(const std::string &c_decl, const char *encodedKind) : ValueType(ValueTypeKind::literal, c_decl), encodedKind(encodedKind) {}
};

class ConstType : public ValueType {
public:
    const ValueType *type;

    ConstType(const ValueType *type) : ValueType(ValueTypeKind::_const, type->c_decl + " const"), type(type) {}
};

class PointerType : public ValueType {
public:
    const ValueType *type;

    PointerType(const ValueType *type) : ValueType(ValueTypeKind::pointer, type->c_decl + "*"), type(type) {}
};

class IntPointerType : public ValueType {
public:
    IntPointerType(const std::string &c_decl) : ValueType(ValueTypeKind::int_pointer, c_decl) {}
};

class ObjPointerType : public ValueType {
public:
    const ValueType *type;

    ObjPointerType(const ValueType *type) : ValueType(ValueTypeKind::obj_pointer, type->c_decl + "*"), type(type) {}
};

class LinearPointerType : public ValueType {
public:
    const ValueType *type;
    const char *size;

    LinearPointerType(const ValueType *type, const char *size) : ValueType(ValueTypeKind::linear_pointer, type->c_decl + "*"), type(type), size(size) {}
};

class ReferenceType : public ValueType {
public:
    const ValueType *type;

    ReferenceType(const ValueType *type) : ValueType(ValueTypeKind::reference, type->c_decl + "&"), type(type) {}
};

class HandleType : public ValueType {
public:
    const ValueType *type;
    const char *name;
    const char *range;
    const ValueType *key_type;
    const char *key_name;

    HandleType(const ValueType *type, const char *name, const char *range, const ValueType *key_type, const char *key_name)
        : ValueType(ValueTypeKind::handle, type->c_decl), type(type), name(name), range(range), key_type(key_type), key_name(key_name) {}
};

class EnumType : public ValueType {
public:
    std::vector<const char *> values;

    EnumType(const char *name, const std::vector<const char *> &values) : ValueType(ValueTypeKind::_enum, name), values(values) {}
};

class BitmaskType : public ValueType {
public:
    const ValueType *type;
    std::vector<const char *> values;

    BitmaskType(const ValueType *type, const std::vector<const char *> &values) : ValueType(ValueTypeKind::bitmask, type->c_decl), type(type), values(values) {}
};

class ArrayType : public ValueType {
public:
    const ValueType *type;
    const char *length;

    ArrayType(const ValueType *type, const char *length) : ValueType(ValueTypeKind::array, type->c_decl + "*"), type(type), length(length) {}
};

class AttribArrayType : public ValueType {
public:
    const ValueType *type;
    std::vector<const ValueType *> valueTypes;
    const char *terminator;

    AttribArrayType(const ValueType *type, const std::vector<const ValueType *> &valueTypes, const char *terminator)
        : ValueType(ValueTypeKind::attrib_array, type->c_decl), type(type), valueTypes(valueTypes), terminator(terminator) {
    }

};

class BlobType : public ValueType {
public:
    const ValueType *type;
    const char *size;

    BlobType(const ValueType *type, const char *size) : ValueType(ValueTypeKind::blob, type->c_decl + "*"), type(type), size(size) {}
};

struct StructMemberType {
    const ValueType *type;
    const char *name;
};

class StructType : public ValueType {
public:
    const char *name;
    std::vector<StructMemberType> members;

    StructType(const char *name, const std::vector<StructMemberType> &members) : ValueType(ValueTypeKind::_struct, name), name(name), members(members) {}
};

class AliasType : public ValueType {
public:
    const char *name;
    const ValueType *type;

    AliasType(const char *name, const ValueType *type) : ValueType(ValueTypeKind::alias, name), name(name), type(type) {}
};

class StringType : public ValueType {
public:
    const ValueType *type;
    const char *length;
    bool wide;

    StringType(const ValueType *type, const char *length, bool wide)
        : ValueType(ValueTypeKind::string, type->c_decl + "*"), type(type), length(length), wide(wide) {
    }
};

class OpaqueType : public ValueType {
public:
    OpaqueType(const std::string &c_decl) : ValueType(ValueTypeKind::opaque, c_decl) {}
};

class PolymorphicType : public ValueType {
public:
    const char *switchExpr;
    std::vector<const ValueType *> switchTypes;
    const ValueType *defaultType;
    bool contextLess;

    PolymorphicType(const char *switchExpr, const std::vector<const ValueType *> &switchTypes, const ValueType *defaultType, bool contextLess)
        : ValueType(ValueTypeKind::polymorphic, defaultType ? defaultType->c_decl : ""), switchExpr(switchExpr), switchTypes(switchTypes),
          defaultType(defaultType), contextLess(contextLess) {
    }
};

// TODO: Interface

struct ArgType {
    const ValueType *type;
    const char *name;
    bool input;
    bool output;
};

struct FunctionType {
    const ValueType *return_type;
    std::vector<ArgType> parameter_types; 
};


} /* namespace retrace */

