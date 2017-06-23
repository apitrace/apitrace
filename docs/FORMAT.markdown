# Trace binary format specification #

This document specifies the binary format of trace streams.

## Compression ##

Trace streams are not written verbatim to file, but compressed.

By default traces are compressed with [Snappy](https://github.com/google/snappy)
(see below for details).  Previously they used to be compressed with gzip.  And
recently it also possible to have them compressed with
[Brotli](https://github.com/google/brotli), though this is mostly intended for
space savings on large databases of trace files.

`apitrace repack` utility can be used to recompress the stream without any loss.

### Snappy ###

The used Snappy format is different from the standard _Snappy framing format_,
because it predates it.

    file = header chunk*

    header = 'a' 't'

    chunk = compressed_length compressed_data

    compressed_length = uint32  // length of compressed data in little endian
    compressed_data = byte*


## Versions ##

We keep backwards compatibility reading old traces, i.e., it should always be
possible to parse and retrace old trace files.

The trace version number refers not only to changes in the binary format
representation, but also semantic changes in the way certain functions should
be retraced.

| `version_no` | Changes |
| ------------ | ------- |
| 0 | initial implementation |
| 1 | support for OpenGL user arrays as blobs (whereas before calls that operated with user memory instead of VBOs should be ignored) |
| 2 | malloc/free memory calls |
| 3 | enums signatures with the whole set of name/value pairs |
| 4 | call enter events include thread no |
| 5 | support for call backtraces |
| 6 | unicode strings; semantic version; properties |

Writing/editing old traces is not supported however.  An older version of
apitrace should be used in such circumstances.


## Basic types ##

| Type | Description |
| ---- | ----------- |
| `byte` | Raw byte. |
| `uint` | Variable-length unsigned integer, where the most significant bit is zero for last byte or non-zero if more bytes follow; the 7 least significant bits of each byte are used for the integer's bits, in little-endian order. |
| `float` | 32 bits single precision floating point number |
| `double` | 64 bits single precision floating point number |

Strings are length-prefixed.  The trailing zero is implied, not appearing neither in the length prefix nor in the raw bytes.

    string = count byte*

    count = uint


## Grammar ##

The trace consists of a small header with version information, followed by an
arbitrary number of events.

    trace = header event*

    header = version_no semantic_version_no properties  // version_no >= 6
           | version_no                                 // version_no < 6

    version_no = uint
    semantic_version_no = uint


### Properties ###

Properties are used to describe characteristics of the process, OS, hardware, etc.

    properties = property+

    property = property_name property_value
             | empty_string                  // terminator
    property_name = string
    property_value = string


### Calls ###

Calls consist of an enter and leave event pair.  Calls are numbered from zero,
and the call number is implied for the enter event.

    event = 0x00 thread_no call_sig call_detail+  // enter call (version_no >= 4)
          | 0x00 call_sig call_detail+            // enter call (version_no < 4)
          | 0x01 call_no call_detail+             // leave call

    call_sig = id function_name count arg_name*  // first occurrence
             | id                                // follow-on occurrences

    call_detail = 0x00                  // terminator
                | 0x01 arg_no value     // argument value
                | 0x02 value            // return value
                | 0x03 thread_no        // thread number (version_no < 4)
                | 0x04 count frame*     // stack backtrace

    arg_name = string
    function_name = string

    arg_no = uint
    call_no = uint
    thread_no = uint

    id = uint

### Values ###

    value = 0x00                    // null pointer
          | 0x01                    // false value
          | 0x02                    // true
          | 0x03 uint               // negative integer
          | 0x04 uint               // positive integer value
          | 0x05 float              // single-precision floating point value
          | 0x06 double             // double-precision floating point value
          | 0x07 string             // character string value (zero terminator implied)
          | 0x08 string             // binary blob
          | 0x09 enum_sig value     // enumeration (version_no >= 3)
          | 0x09 string value       // enumeration (version_no < 3)
          | 0x0a bitmask_sig value  // integer bitmask
          | 0x0b count value*       // array
          | 0x0c struct_sig value*  // structure
          | 0x0d uint               // opaque pointer
          | 0x0e value value        // human-machine representation
          | 0x0f wstring            // wide character string value (zero terminator implied)

    enum_sig = id count (name value)+  // first occurrence
             | id                      // follow-on occurrences

    bitmask_sig = id count (name uint)+  // first occurrence
                | id                     // follow-on occurrences

    struct_sig = id struct_name count member_name*  // first occurrence
               | id                                 // follow-on occurrences

    name = string
    struct_name = string
    member_name = string

    wstring = count uint*

### Backtraces ###

    frame = id frame_detail+  // first occurrence
          | id                // follow-on occurrences

    frame_detail = 0x00         // terminator
                 | 0x01 string  // module name
                 | 0x02 string  // function name
                 | 0x03 string  // source file name
                 | 0x04 uint    // source line number
                 | 0x05 uint    // byte offset from module start
