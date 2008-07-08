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


#include "log.hpp"


File::File(const TCHAR *szName, const TCHAR *szExtension) {
    m_hFile = INVALID_HANDLE_VALUE;
    Open(szName, szExtension);
}

File::~File() {
    Close();
}

void File::Open(const TCHAR *szName, const TCHAR *szExtension) {
    Close();
    
    DWORD dwCounter = 0;
    do {
        if(dwCounter)
            _sntprintf(szFileName, MAX_PATH, TEXT("%s.%u.%s"), szName, dwCounter, szExtension);
        else
            _sntprintf(szFileName, MAX_PATH, TEXT("%s.%s"), szName, szExtension);

        m_hFile = CreateFile(szFileName,
                             GENERIC_WRITE,
                             FILE_SHARE_WRITE,
                             NULL,
                             CREATE_NEW,
                             FILE_ATTRIBUTE_NORMAL,
                             NULL);
        ++dwCounter;
    } while(m_hFile == INVALID_HANDLE_VALUE && GetLastError() == ERROR_FILE_EXISTS);
}

void File::ReOpen(void) {
    Close();
    
    m_hFile = CreateFile(szFileName,
                         GENERIC_WRITE,
                         0,
                         NULL,
                         OPEN_EXISTING,
                         FILE_ATTRIBUTE_NORMAL,
                         NULL);
}

void File::Close(void) {
    if(m_hFile != INVALID_HANDLE_VALUE) {
        CloseHandle(m_hFile);
        m_hFile = INVALID_HANDLE_VALUE;
    }
}

void File::Write(const char *szText) {
    if(m_hFile == INVALID_HANDLE_VALUE)
        return;
    
    DWORD dwBytesToWrite = (DWORD)strlen(szText);
    DWORD dwBytesWritten = 0;
    
    while (dwBytesWritten < dwBytesToWrite) {
        OVERLAPPED overlapped;
        memset(&overlapped, 0, sizeof(OVERLAPPED));

        /* Write to end of file */
        overlapped.Offset = 0xffffffff;
        overlapped.OffsetHigh = 0xffffffff;
        
        if(WriteFile(m_hFile,
                     szText + dwBytesWritten,
                     dwBytesToWrite - dwBytesWritten,
                     &dwBytesWritten,
                     &overlapped) == FALSE) {
            Close();
            Open(TEXT("extra"), TEXT("xml"));
            return;
        }
    }
}


Log::Log(const TCHAR *szName) : File(szName, TEXT("xml")) {
    Write("<?xml version='1.0' encoding='UTF-8'?>");
    NewLine();
    Write("<?xml-stylesheet type='text/xsl' href='d3dtrace.xsl'?>");
    NewLine();
    Write("<trace>");
    NewLine();
}

Log::~Log() {
    Write("</trace>");
    NewLine();
}

void Log::NewLine(void) {
    Write("\r\n");
}

void Log::Tag(const char *name) {
    Write("<");
    Write(name);
    Write("/>");
}

void Log::BeginTag(const char *name) {
    Write("<");
    Write(name);
    Write(">");
}

void Log::BeginTag(const char *name, 
              const char *attr1, const char *value1) {
    Write("<");
    Write(name);
    Write(" ");
    Write(attr1);
    Write("=\"");
    Escape(value1);
    Write("\">");
}

void Log::BeginTag(const char *name, 
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

void Log::EndTag(const char *name) {
    Write("</");
    Write(name);
    Write(">");
}

void Log::Text(const char *text) {
    Escape(text);
}

void Log::TextF(const char *format, ...) {
    char szBuffer[4196];
    va_list ap;
    va_start(ap, format);
    vsnprintf(szBuffer, sizeof(szBuffer), format, ap);
    va_end(ap);
    Escape(szBuffer);
}

void Log::BeginCall(const char *function) {
    Write("\t");
    BeginTag("call", "name", function);
    NewLine();
}

void Log::EndCall(void) {
    Write("\t");
    EndTag("call");
    NewLine();
}

void Log::BeginParam(const char *name, const char *type) {
    Write("\t\t");
    BeginTag("param", "name", name, "type", type);
}

void Log::EndParam(void) {
    EndTag("param");
    NewLine();
}

void Log::BeginReturn(const char *type) {
    Write("\t\t");
    BeginTag("return", "type", type);
}

void Log::EndReturn(void) {
    EndTag("return");
    NewLine();
}

void Log::Escape(const char *s) {
    /* FIXME */
    Write(s);
}


Log * g_pLog = NULL;
