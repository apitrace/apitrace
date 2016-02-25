/**************************************************************************
 *
 * Copyright 2012-2013 VMware, Inc.
 * All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sub license, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice (including the
 * next paragraph) shall be included in all copies or substantial portions
 * of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT.
 * IN NO EVENT SHALL VMWARE AND/OR ITS SUPPLIERS BE LIABLE FOR
 * ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 **************************************************************************/

#ifndef _MINGW_TWEAKS_H_
#define _MINGW_TWEAKS_H_

#ifndef __MINGW32__
#error "This header should only be included for MinGW"
#endif


// workaround http://sourceforge.net/tracker/index.php?func=detail&aid=3067012&group_id=202880&atid=983354
#include <malloc.h>


#include <string.h>
#define memcpy_s(dest, numberOfElements, src, count) memcpy(dest, src, count)


#include "winsdk_compat.h"


#ifndef ERROR_FILE_TOO_LARGE
#define ERROR_FILE_TOO_LARGE 223L
#endif


#endif /* _MINGW_TWEAKS_H_ */
