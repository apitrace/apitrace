/**************************************************************************
 *
 * Copyright 2007-2009 VMware, Inc.
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


#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <windows.h>

#include <zlib.h>

#include "log.hpp"


#ifdef WIN32
#ifndef PATH_MAX
#define PATH_MAX _MAX_PATH
#endif
#ifndef snprintf
#define snprintf _snprintf
#endif
#ifndef vsnprintf
#define vsnprintf _vsnprintf
#endif
#endif


namespace Log {


static gzFile g_gzFile = NULL;
static char g_szFileName[PATH_MAX];
static CRITICAL_SECTION CriticalSection;

static void _Close(void) {
    if(g_gzFile != NULL) {
        gzclose(g_gzFile);
        g_gzFile = NULL;
        DeleteCriticalSection(&CriticalSection);
    }
}

static void _Open(const char *szName, const char *szExtension) {
    _Close();
    
    static unsigned dwCounter = 0;

    char szProcessPath[PATH_MAX];
    char *lpProcessName;
    char *lpProcessExt;

    GetModuleFileNameA(NULL, szProcessPath, sizeof(szProcessPath)/sizeof(szProcessPath[0]));

    lpProcessName = strrchr(szProcessPath, '\\');
    lpProcessName = lpProcessName ? lpProcessName + 1 : szProcessPath;
    lpProcessExt = strrchr(lpProcessName, '.');
    if(lpProcessExt)
	*lpProcessExt = '\0';

    for(;;) {
        FILE *file;
        
        if(dwCounter)
            snprintf(g_szFileName, PATH_MAX, "%s.%s.%u.%s.gz", lpProcessName, szName, dwCounter, szExtension);
        else
            snprintf(g_szFileName, PATH_MAX, "%s.%s.%s.gz", lpProcessName, szName, szExtension);
        
        file = fopen(g_szFileName, "rb");
        if(file == NULL)
            break;
        
        fclose(file);
        
        ++dwCounter;
    }

    g_gzFile = gzopen(g_szFileName, "wb");
    InitializeCriticalSection(&CriticalSection);
}

static inline void _ReOpen(void) {
    /* XXX */
}

static inline void Write(const char *sBuffer, size_t dwBytesToWrite) {
    if(g_gzFile == NULL)
        return;
    
    gzwrite(g_gzFile, sBuffer, dwBytesToWrite);
}

static inline void Write(const char *szText) {
    Write(szText, strlen(szText));
}

static inline void Write(char c) 
{
    Write(&c, 1);
}

static inline void
WriteF(const char *format, ...) 
{
    char szBuffer[4096];
    va_list ap;
    va_start(ap, format);
    vsnprintf(szBuffer, sizeof(szBuffer), format, ap);
    va_end(ap);
    Write(szBuffer);
}

static inline void 
Escape(char c) 
{
    switch(c) {
    case '&':
        Write("&amp;");
        break;
    case '<':
        Write("&lt;");
        break;
    case '>':
        Write("&gt;");
        break;
    case '"':
        Write("&quot;");
        break;
    case '\'':
        Write("&apos;");
        break;
    default:
        Write(c);
    }
}

static inline void 
Escape(const char *s)
{
    unsigned char c;
    while((c = *s++) != 0) {
        Escape(c);
    }
}

static inline void
EscapeF(const char *format, ...)
{
    char szBuffer[4096];
    va_list ap;
    va_start(ap, format);
    vsnprintf(szBuffer, sizeof(szBuffer), format, ap);
    va_end(ap);
    Escape(szBuffer);
}

static inline void 
Indent(unsigned level) {
    for(unsigned i = 0; i < level; ++i)
        Write("\t");
}

static inline void 
NewLine(void) {
    Write("\r\n");
}

static inline void 
Tag(const char *name) {
    Write("<");
    Write(name);
    Write("/>");
}

static inline void 
BeginTag(const char *name) {
    Write("<");
    Write(name);
    Write(">");
}

static inline void 
BeginTag(const char *name, 
         const char *attr1, const char *value1) {
    Write("<");
    Write(name);
    Write(" ");
    Write(attr1);
    Write("=\"");
    Escape(value1);
    Write("\">");
}

static inline void 
BeginTag(const char *name, 
         const char *attr1, const char *value1,
         const char *attr2, const char *value2) {
    Write("<");
    Write(name);
    Write(" ");
    Write(attr1);
    Write("=\"");
    Escape(value1);
    Write("\" ");
    Write(attr2);
    Write("=\"");
    Escape(value2);
    Write("\">");
}

static inline void 
BeginTag(const char *name, 
              const char *attr1, const char *value1,
              const char *attr2, const char *value2,
              const char *attr3, const char *value3) {
    Write("<");
    Write(name);
    Write(" ");
    Write(attr1);
    Write("=\"");
    Escape(value1);
    Write("\" ");
    Write(attr2);
    Write("=\"");
    Escape(value2);
    Write("\" ");
    Write(attr3);
    Write("=\"");
    Escape(value3);
    Write("\">");
}

static inline void
EndTag(const char *name) {
    Write("</");
    Write(name);
    Write(">");
}

void Open(const char *name) {
    _Open(name, "xml");
    Write("<?xml version='1.0' encoding='UTF-8'?>");
    NewLine();
    Write("<?xml-stylesheet type='text/xsl' href='apitrace.xsl'?>");
    NewLine();
    BeginTag("trace");
    NewLine();
}

void ReOpen(void) {
    _ReOpen();
}

void Close(void) {
    EndTag("trace");
    NewLine();
    _Close();
}

void Text(const char *text) {
    Escape(text);
}

void TextF(const char *format, ...) {
    char szBuffer[4096];
    va_list ap;
    va_start(ap, format);
    vsnprintf(szBuffer, sizeof(szBuffer), format, ap);
    va_end(ap);
    Escape(szBuffer);
}

static LARGE_INTEGER frequency = {0};
static LARGE_INTEGER startcounter;

void BeginCall(const char *function) {
    EnterCriticalSection(&CriticalSection); 
    Indent(1);
    BeginTag("call", "name", function);
    NewLine();

    if(!frequency.QuadPart)
	QueryPerformanceFrequency(&frequency);
    
    QueryPerformanceCounter(&startcounter);
}

void EndCall(void) {
    LARGE_INTEGER endcounter;
    LONGLONG usecs;

    QueryPerformanceCounter(&endcounter);
    usecs = (endcounter.QuadPart - startcounter.QuadPart)*1000000/frequency.QuadPart;

    Indent(2);
    BeginTag("duration");
    TextF("%llu", usecs);
    EndTag("duration");
    NewLine();

    Indent(1);
    EndTag("call");
    NewLine();
    gzflush(g_gzFile, Z_SYNC_FLUSH);
    LeaveCriticalSection(&CriticalSection); 
}

void BeginArg(const char *type, const char *name) {
    Indent(2);
    BeginTag("arg", "type", type, "name", name);
}

void EndArg(void) {
    EndTag("arg");
    NewLine();
}

void BeginReturn(const char *type) {
    Indent(2);
    BeginTag("ret", "type", type);
}

void EndReturn(void) {
    EndTag("ret");
    NewLine();
}

void BeginElement(const char *type, const char *name) {
    BeginTag("elem", "type", type, "name", name);
}

void BeginElement(const char *type) {
    BeginTag("elem", "type", type);
}

void EndElement(void) {
    EndTag("elem");
}

void BeginReference(const char *type, const void *addr) {
    char saddr[256];
    snprintf(saddr, sizeof(saddr), "%p", addr);
    BeginTag("ref", "type", type, "addr", saddr);
}

void EndReference(void) {
    EndTag("ref");
}

void DumpString(const char *str) {
    const unsigned char *p = (const unsigned char *)str;
    if (!str) {
        Write("NULL");
        return;
    }
    Write("\"");
    unsigned char c;
    while((c = *p++) != 0) {
        if(c == '\"')
            Write("\\\"");
        else if(c == '\\')
            Write("\\\\");
        else if(c >= 0x20 && c <= 0x7e)
            Write(c);
        else if(c == '\t')
            Write("&#09;");
        else if(c == '\r')
            Write("&#13;");
        else if(c == '\n')
            Write("&#10;");
        else {
            unsigned char octal0 = c & 0x7;
            unsigned char octal1 = (c >> 3) & 0x7;
            unsigned char octal2 = (c >> 3) & 0x7;
            if(octal2)
                WriteF("\\%u%u%u", octal2, octal1, octal0);
            else if(octal1)
                WriteF("\\%u%u", octal1, octal0);
            else
                WriteF("\\%u", octal0);
        }
    }
    Write("\"");
}

void DumpWString(const wchar_t *str) {
    const wchar_t *p = str;
    if (!str) {
        Write("NULL");
        return;
    }
    Write("L\"");
    wchar_t c;
    while((c = *p++) != 0) {
        if(c == '\"')
            Write("\\\"");
        else if(c == '\\')
            Write("\\\\");
        else if(c >= 0x20 && c <= 0x7e)
            Write((char)c);
        else if(c == '\t')
            Write("&#09;");
        else if(c == '\r')
            Write("&#13;");
        else if(c == '\n')
            Write("&#10;");
        else {
            unsigned octal0 = c & 0x7;
            unsigned octal1 = (c >> 3) & 0x7;
            unsigned octal2 = (c >> 3) & 0x7;
            if(octal2)
                WriteF("\\%u%u%u", octal2, octal1, octal0);
            else if(octal1)
                WriteF("\\%u%u", octal1, octal0);
            else
                WriteF("\\%u", octal0);
        }
    }
    Write("\"");
}

} /* namespace Log */
