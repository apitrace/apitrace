
#ifndef LOG_HPP_
#define LOG_HPP_

#include <windows.h>

#include <d3d8.h>

#include <stdio.h>


class Log
{
public:
    Log(const char *filename) {
        file = fopen(filename, "wt");
        write("<?xml version='1.0' encoding='UTF-8'?>\n");
        write("<?xml-stylesheet type='text/xsl' href='d3dtrace.xsl'?>\n");
        write("<trace>\n");
    }
    
    ~Log() {
        write("</trace>\n");
        fclose(file);
    }
    
    void write(const char *s) {
        fputs(s, file);
    }
    
    void writef(const char *f, ...) {
        va_list ap;
        va_start(ap, f);
        vfprintf(file, f, ap);
        va_end(ap);
    }
    
    void eol(void) {
        fputs("\n", file);
    }
    
    void tag(const char *name) {
        write("<");
        write(name);
        write(">");
    }
    
    void tag_begin(const char *name) {
        write("<");
        write(name);
    }
    
    void tag_attr(const char *name, const char *value) {
        write(" ");
        write(name);
        write("=\"");
        write(value);
        write("\"");
    }
    
    void tag_end(void) {
        write(">");
    }
    
    void tag_close(const char *name) {
        write("</");
        write(name);
        write(">");
    }
    
    void call_begin(const char *function) {
        write("\t");
        tag_begin("call");
        tag_attr("name", function);
        tag_end();
        eol();
    }
    
    void call_end() {
        write("\t");
        tag_close("call");
        eol();
    }
    
    void param_begin(const char *type, const char *name) {
        write("\t\t");
        tag_begin("param");
        tag_attr("type", type);
        tag_attr("name", name);
        tag_end();
    }
    
    void param_end(void) {
        tag_close("param");
        eol();
    }
    
    void param_uint(const char *name, UINT value) {
        param_begin("UINT", name);
        writef("%u", value);
        param_end();
    }
    
    void param_dword(const char *name, DWORD value) {
        param_begin("DWORD", name);
        writef("0x%08lx", value);
        param_end();
    }
    
protected:
    FILE *file;
};


#endif /* LOG_HPP_ */
