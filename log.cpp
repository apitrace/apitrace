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

#include <zlib.h>

#include "os.hpp"
#include "log.hpp"


namespace Log {


static gzFile g_gzFile = NULL;
static void _Close(void) {
    if(g_gzFile != NULL) {
        gzclose(g_gzFile);
        g_gzFile = NULL;
    }
}

static void _Open(const char *szName, const char *szExtension) {
    _Close();
    
    static unsigned dwCounter = 0;

    char szProcessName[PATH_MAX];
    char szFileName[PATH_MAX];

    OS::GetProcessName(szProcessName, PATH_MAX);

    for(;;) {
        FILE *file;
        
        if(dwCounter)
            snprintf(szFileName, PATH_MAX, "%s.%s.%u.%s.gz", szProcessName, szName, dwCounter, szExtension);
        else
            snprintf(szFileName, PATH_MAX, "%s.%s.%s.gz", szProcessName, szName, szExtension);
        
        file = fopen(szFileName, "rb");
        if(file == NULL)
            break;
        
        fclose(file);
        
        ++dwCounter;
    }

    fprintf(stderr, "Logging to %s\n", szFileName);
    g_gzFile = gzopen(szFileName, "wb");
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
Escape(wchar_t c) 
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
    case '\t':
        Write("&#09;");
        break;
    case '\r':
        Write("&#13;");
        break;
    case '\n':
        Write("&#10;");
        break;
    default:
        if (c >= 0x20 && c <= 0x7e) {
            Write((char)c);
        } else {
            Write('.');
        }
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
Escape(const wchar_t *s)
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

void BeginCall(const char *function) {
    OS::AcquireMutex();
    Indent(1);
    BeginTag("call", "name", function);
    NewLine();
}

void EndCall(void) {
    Indent(1);
    EndTag("call");
    NewLine();
    gzflush(g_gzFile, Z_SYNC_FLUSH);
    OS::ReleaseMutex();
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

void BeginArray(const char *type, size_t length)
{
    BeginTag("array", "type", type);
}

void EndArray(void)
{
    EndTag("array");
}

void BeginElement(const char *type)
{
    BeginTag("elem", "type", type);
}

void EndElement(void)
{
    EndTag("elem");
}

void BeginStruct(const char *type)
{
    BeginTag("struct", "type", type);
}

void EndStruct(void)
{
    EndTag("struct");
}

void BeginMember(const char *type, const char *name)
{
    BeginTag("member", "type", type, "name", name);
}

void EndMember(void)
{
    EndTag("member");
}

void BeginBitmask(const char *type)
{
    BeginTag("bitmask");
}

void EndBitmask(void)
{
    EndTag("bitmask");
}

void BeginReference(const char *type, const void *addr)
{
    char saddr[256];
    snprintf(saddr, sizeof(saddr), "%p", addr);
    BeginTag("ref", "type", type, "addr", saddr);
}

void EndReference(void)
{
    EndTag("ref");
}

void LiteralBool(bool value)
{
    BeginTag("bool");
    WriteF("%u", value ? 0 : 1);
    EndTag("bool");
}

void LiteralSInt(signed long long value)
{
    BeginTag("int");
    WriteF("%lli", value);
    EndTag("int");
}

void LiteralUInt(unsigned long long value)
{
    BeginTag("uint");
    WriteF("%llu", value);
    EndTag("uint");
}

void LiteralFloat(double value)
{
    BeginTag("float");
    WriteF("%f", value);
    EndTag("float");
}

void LiteralString(const char *str)
{
    if (!str) {
        LiteralNull();
        return;
    }
    BeginTag("string");
    Escape(str);
    EndTag("string");
}

void LiteralWString(const wchar_t *str)
{
    if (!str) {
        LiteralNull();
        return;
    }
    BeginTag("wstring");
    Escape(str);
    EndTag("wstring");
}
    
void LiteralNamedConstant(const char *str)
{
    BeginTag("const");
    Escape(str);
    EndTag("const");
}

void LiteralOpaque(const void *addr)
{
    char saddr[256];
    if (!addr) {
        LiteralNull();
        return;
    }
    snprintf(saddr, sizeof(saddr), "%p", addr);
    BeginTag("opaque", "addr", saddr);
    EndTag("opaque");
}

void LiteralNull(void)
{
    Tag("null");
}

} /* namespace Log */
