/****************************************************************************
 *
 * Copyright 2008 Jose Fonseca
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


#include <stdio.h>

#include "log.hpp"


namespace Log {


static HANDLE g_hFile = INVALID_HANDLE_VALUE;
static TCHAR g_szFileName[MAX_PATH];

static void _Close(void) {
    if(g_hFile != INVALID_HANDLE_VALUE) {
        CloseHandle(g_hFile);
        g_hFile = INVALID_HANDLE_VALUE;
    }
}

static void _Open(const TCHAR *szName, const TCHAR *szExtension) {
    _Close();
    
    DWORD dwCounter = 0;
    do {
        if(dwCounter)
            _sntprintf(g_szFileName, MAX_PATH, TEXT("%s.%u.%s"), szName, dwCounter, szExtension);
        else
            _sntprintf(g_szFileName, MAX_PATH, TEXT("%s.%s"), szName, szExtension);

        g_hFile = CreateFile(g_szFileName,
                             GENERIC_WRITE,
                             FILE_SHARE_WRITE,
                             NULL,
                             CREATE_NEW,
                             FILE_ATTRIBUTE_NORMAL,
                             NULL);
        ++dwCounter;
    } while(g_hFile == INVALID_HANDLE_VALUE && GetLastError() == ERROR_FILE_EXISTS);
}

static void _ReOpen(void) {
    _Close();
    
    g_hFile = CreateFile(g_szFileName,
                         GENERIC_WRITE,
                         FILE_SHARE_WRITE,
                         NULL,
                         OPEN_EXISTING,
                         FILE_ATTRIBUTE_NORMAL,
                         NULL);
}

static void Write(const char *sBuffer, DWORD dwBytesToWrite) {
    if(g_hFile == INVALID_HANDLE_VALUE)
        return;
    
    DWORD dwBytesWritten = 0;
    
    while (dwBytesWritten < dwBytesToWrite) {
        OVERLAPPED overlapped;
        memset(&overlapped, 0, sizeof(OVERLAPPED));

        /* Write to end of file */
        overlapped.Offset = 0xffffffff;
        overlapped.OffsetHigh = 0xffffffff;
        
        if(WriteFile(g_hFile,
                     sBuffer + dwBytesWritten,
                     dwBytesToWrite - dwBytesWritten,
                     &dwBytesWritten,
                     &overlapped) == FALSE) {
            _Close();
            _Open(TEXT("extra"), TEXT("xml"));
            return;
        }
    }
}

static void Write(const char *szText) {
    Write(szText, (DWORD)strlen(szText));
}

void Open(const TCHAR *szName) {
    _Open(szName, TEXT("xml"));
    Write("<?xml version='1.0' encoding='UTF-8'?>");
    NewLine();
    Write("<?xml-stylesheet type='text/xsl' href='d3dtrace.xsl'?>");
    NewLine();
    Write("<trace>");
    NewLine();
}

void ReOpen(void) {
    _ReOpen();
}

void Close(void) {
    Write("</trace>");
    NewLine();
    _Close();
}

static void Escape(const char *s) {
    /* FIXME */
    Write(s);
}


DWORD g_dwIndent = 0;

void NewLine(void) {
    Write("\r\n");
    for(unsigned i = 0; i < g_dwIndent; ++i)
        Write("\t");        
}

void Tag(const char *name) {
    Write("<");
    Write(name);
    Write("/>");
}

void BeginTag(const char *name) {
    Write("<");
    Write(name);
    Write(">");
    ++g_dwIndent;
}

void BeginTag(const char *name, 
              const char *attr1, const char *value1) {
    Write("<");
    Write(name);
    Write(" ");
    Write(attr1);
    Write("=\"");
    Escape(value1);
    Write("\">");
    ++g_dwIndent;
}

void BeginTag(const char *name, 
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
    ++g_dwIndent;
}

void BeginTag(const char *name, 
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
    ++g_dwIndent;
}

void EndTag(const char *name) {
    --g_dwIndent;
    Write("</");
    Write(name);
    Write(">");
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
    BeginTag("call", "name", function);
    NewLine();
}

void EndCall(void) {
    EndTag("call");
    NewLine();
}

void BeginArg(const char *type, const char *name) {
    BeginTag("arg", "type", type, "name", name);
}

void EndArg(void) {
    EndTag("arg");
    NewLine();
}

void BeginReturn(const char *type) {
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
    _snprintf(saddr, sizeof(saddr), "%p", addr);
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
