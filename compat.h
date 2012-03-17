/**************************************************************************
 *
 * Copyright 2008-2009 VMware, Inc.
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
 * MinGW compatability macros to allow using recent's DXSDK headers.
 */

#ifdef __MINGW32__

#define __in /**/
#define __out /**/
#define __inout /**/
#define __inout_opt /**/
#define __in_opt /**/
#define __out_opt /**/
#define __in_bcount(x) /**/
#define __out_bcount(x) /**/
#define __in_bcount_opt(x) /**/
#define __out_bcount_opt(x) /**/
#define __in_range(x,y) /**/
#define __in_ecount(x) /**/
#define __out_ecount(x) /**/
#define __in_ecount_opt(x) /**/
#define __out_ecount_opt(x) /**/
#define __field_bcount(x) /**/
#define __field_ecount(x) /**/
#define __field_ecount_full(x) /**/
#define __inline static __inline__
#define __deref_out /**/
#define __deref_out_opt /**/
#define __deref_out_bcount(x) /**/
#define __maybenull /**/
#define __in_z /**/
#define __in_z_opt /**/
#define __out_z /**/
#define __out_ecount_z(x) /**/
#define __nullterminated /**/
#define __notnull /**/
#define __field_ecount_opt(x) /**/
#define __range(x,y) /**/
#define __out_ecount_part_opt(x,y) /**/

#ifndef DECLSPEC_DEPRECATED
#define DECLSPEC_DEPRECATED /**/
#endif

#ifndef DECLSPEC_NOVTABLE
#define DECLSPEC_NOVTABLE /**/
#endif

#ifndef __MINGW64_VERSION_MAJOR
#define INT8 signed char
#define UINT8 unsigned char
#define INT16 signed short
#define UINT16 unsigned short
#endif

#endif /* __MINGW32__ */

