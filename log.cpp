/****************************************************************************
 *
 * Copyright 2008 Tungsten Graphics, Inc.
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 ****************************************************************************/


#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

static void _Close(void) {
    if(g_gzFile != NULL) {
        gzclose(g_gzFile);
        g_gzFile = NULL;
    }
}

static void _Open(const char *szName, const char *szExtension) {
    _Close();
    
    static unsigned dwCounter = 0;

    for(;;) {
        FILE *file;
        
        if(dwCounter)
            snprintf(g_szFileName, PATH_MAX, "%s.%u.%s.gz", szName, dwCounter, szExtension);
        else
            snprintf(g_szFileName, PATH_MAX, "%s.%s.gz", szName, szExtension);
        
        file = fopen(g_szFileName, "rb");
        if(file == NULL)
            break;
        
        fclose(file);
        
        ++dwCounter;
    }

    g_gzFile = gzopen(g_szFileName, "wb");
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

static inline void 
Escape(const char *s) {
    /* FIXME */
    Write(s);
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
    Write("<?xml-stylesheet type='text/xsl' href='d3dtrace.xsl'?>");
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

static void TextChar(char c) {
    char szText[2];
    szText[0] = c;
    szText[1] = 0;
    Text(szText);
}

void TextF(const char *format, ...) {
    char szBuffer[4196];
    va_list ap;
    va_start(ap, format);
    vsnprintf(szBuffer, sizeof(szBuffer), format, ap);
    va_end(ap);
    Text(szBuffer);
}

void BeginCall(const char *function) {
    Indent(1);
    BeginTag("call", "name", function);
    NewLine();
}

void EndCall(void) {
    Indent(1);
    EndTag("call");
    NewLine();
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
    Log::Text("\"");
    unsigned char c;
    while((c = *p++) != 0) {
        if(c == '\"')
            Text("\\\"");
        else if(c == '\\')
            Text("\\\\");
        else if(c >= 0x20 && c <= 0x7e)
            TextChar(c);
        else if(c == '\t')
            Text("\\t");
        else if(c == '\r')
            Text("\\r");
        else if(c == '\n')
            Text("\\n");
        else {
            unsigned char octal0 = c & 0x7;
            unsigned char octal1 = (c >> 3) & 0x7;
            unsigned char octal2 = (c >> 3) & 0x7;
            if(octal2)
                TextF("\\%u%u%u", octal2, octal1, octal0);
            else if(octal1)
                TextF("\\%u%u", octal1, octal0);
            else
                TextF("\\%u", octal0);
        }
    }
    Log::Text("\"");
}

} /* namespace Log */
