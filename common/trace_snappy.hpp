/**************************************************************************
 *
 * Copyright 2015 VMware, Inc
 * Copyright 2011 Zack Rusin
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
 * Snappy file format.
 * -------------------
 *
 * Snappy at its core is just a compressoin algorithm so we're
 * creating a new file format which uses snappy compression
 * to hold the trace data.
 *
 * The file is composed of a number of chunks, they are:
 * chunk {
 *     uint32 - specifying the length of the compressed data
 *     compressed data, in little endian
 * }
 * File can contain any number of such chunks.
 * The default size of an uncompressed chunk is specified in
 * SNAPPY_CHUNK_SIZE.
 *
 * Note:
 * Currently the default size for a a to-be-compressed data is
 * 1mb, meaning that the compressed data will be <= 1mb.
 * The reason it's 1mb is because it seems
 * to offer a pretty good compression/disk io speed ratio
 * but that might change.
 *
 */


#pragma once


#define SNAPPY_BYTE1 'a'
#define SNAPPY_BYTE2 't'


