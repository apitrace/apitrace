
#ifndef _LOG_HPP_
#define _LOG_HPP_

#include <stdio.h>
#include <stdlib.h>


class Log
{
public:
    Log(const char *name) {
        char fileName[_MAX_PATH];
        _snprintf(fileName, _MAX_PATH, "%s.%u.xml", name, rand());
        file = fopen(fileName, "wt");
        Write("<?xml version='1.0' encoding='UTF-8'?>\n");
        Write("<?xml-stylesheet type='text/xsl' href='d3dtrace.xsl'?>\n");
        Write("<trace>\n");
    }
    
    ~Log() {
        Write("</trace>\n");
        fclose(file);
    }
    
    void NewLine(void) {
        Write("\n");
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
        va_list ap;
        va_start(ap, format);
        vfprintf(file, format, ap);
        va_end(ap);
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
    
    void BeginParam(const char *type, const char *name) {
        Write("\t\t");
        BeginTag("param", "type", type, "name", name);
    }
    
    void EndParam(void) {
        EndTag("param");
        NewLine();
    }
    
    void BeginReturn(const char *type, const char *name) {
        Write("\t\t");
        BeginTag("return", "type", type);
    }
    
    void EndReturn(void) {
        EndTag("return");
        NewLine();
    }
    
protected:
    void Escape(const char *s) {
        /* FIXME */
        Write(s);
    }
    
    void Write(const char *s) {
        fputs(s, file);
    }
    
    void WriteF(const char *f, ...) {
        va_list ap;
        va_start(ap, f);
        vfprintf(file, f, ap);
        va_end(ap);
    }

private:
    FILE *file;
};


#endif /* _LOG_HPP_ */
