/**************************************************************************
 *
 * Copyright 2015 VMware, Inc
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


#include "os_crtdbg.hpp"

#include <assert.h>
#include <stdio.h>

#ifdef _WIN32
#include <windows.h>
#include <crtdbg.h>
#endif

#include "os.hpp"


namespace os {


#ifdef _WIN32

static void
invalidParameterHandler(const wchar_t* expression,
                        const wchar_t* function,
                        const wchar_t* file,
                        unsigned int line,
                        uintptr_t pReserved)
{
     fprintf(stderr, "Invalid parameter detected in function %S. File: %S Line: %d\n", function, file, line);
     fprintf(stderr, "Expression: %S\n", expression);
     if (IsDebuggerPresent()) {
         os::breakpoint();
     }
     os::abort();
}

#endif // _WIN32


void
setDebugOutput(Output output)
{

#ifdef _WIN32
    // Disable assertion failure message box
    // http://msdn.microsoft.com/en-us/library/sas1dkb2.aspx
    _set_error_mode(_OUT_TO_STDERR);
#ifdef _MSC_VER
    // Disable abort message box
    // http://msdn.microsoft.com/en-us/library/e631wekh.aspx
    _set_abort_behavior(0, _WRITE_ABORT_MSG);
    // Direct debug reports to stderr
    // https://msdn.microsoft.com/en-us/library/1y71x448.aspx
    for (int reportType = 0; reportType < _CRT_ERRCNT; ++reportType) {
        _CrtSetReportMode(reportType, _CRTDBG_MODE_FILE);
        _CrtSetReportFile(reportType, _CRTDBG_FILE_STDERR);
    }
#endif /* _MSC_VER */
    // Set our own invalid_parameter handler
    // https://msdn.microsoft.com/en-us/library/a9yf33zb.aspx
    _set_invalid_parameter_handler(invalidParameterHandler);
#endif /* _WIN32 */


    assert(output == OUTPUT_STDERR);
}


} /* namespace os */
