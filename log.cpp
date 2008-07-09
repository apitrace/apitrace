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

static void Write(const char *szText) {
    if(g_hFile == INVALID_HANDLE_VALUE)
        return;
    
    DWORD dwBytesToWrite = (DWORD)strlen(szText);
    DWORD dwBytesWritten = 0;
    
    while (dwBytesWritten < dwBytesToWrite) {
        OVERLAPPED overlapped;
        memset(&overlapped, 0, sizeof(OVERLAPPED));

        /* Write to end of file */
        overlapped.Offset = 0xffffffff;
        overlapped.OffsetHigh = 0xffffffff;
        
        if(WriteFile(g_hFile,
                     szText + dwBytesWritten,
                     dwBytesToWrite - dwBytesWritten,
                     &dwBytesWritten,
                     &overlapped) == FALSE) {
            _Close();
            _Open(TEXT("extra"), TEXT("xml"));
            return;
        }
    }
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

void NewLine(void) {
    Write("\r\n");
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
}

void EndTag(const char *name) {
    Write("</");
    Write(name);
    Write(">");
}

void Text(const char *text) {
    Escape(text);
}

void TextF(const char *format, ...) {
    char szBuffer[4196];
    va_list ap;
    va_start(ap, format);
    vsnprintf(szBuffer, sizeof(szBuffer), format, ap);
    va_end(ap);
    Escape(szBuffer);
}

void BeginCall(const char *function) {
    Write("\t");
    BeginTag("call", "name", function);
    NewLine();
}

void EndCall(void) {
    Write("\t");
    EndTag("call");
    NewLine();
}

void BeginParam(const char *name, const char *type) {
    Write("\t\t");
    BeginTag("param", "name", name, "type", type);
}

void EndParam(void) {
    EndTag("param");
    NewLine();
}

void BeginReturn(const char *type) {
    Write("\t\t");
    BeginTag("return", "type", type);
}

void EndReturn(void) {
    EndTag("return");
    NewLine();
}


} /* namespace Log */