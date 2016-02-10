/**************************************************************************
 *
 * Copyright 2015 Brian Paul
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


/*
 * Configuration file support.
 *
 * Used to override glGetString/Integer/etc queries.
 */


/*
 * The format of the file is extremely simple.  An example is given in
 * docs/USAGE.markdown .
 *
 * Future improvements:
 * - An option to simply turn off specific extensions
 *
 */


#include <assert.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <iostream>

#include "os.hpp"
#include "os_string.hpp"
#include "config.hpp"


namespace gltrace {


/**
 * Parse the conf->extensions string to generate an array of individual
 * extension strings which can be queried with glGetStringi().
 */
static void
create_extensions_list(configuration *conf)
{
    // count extensions
    const char *ext = conf->extensions.c_str();
    conf->numExtensions = 0;
    while (1) {
        ext = strstr(ext, "GL_");
        if (!ext)
            break;

        conf->numExtensions++;
        // advance extensions pointer
        while (*ext && *ext != ' ')
            ext++;
    }

    conf->extensionsList =
        (char **) malloc(conf->numExtensions * sizeof(char *));

    // extract individual extension names
    int count = 0;
    const char *extStart = conf->extensions.c_str(), *extEnd;
    while (1) {
        extStart = strstr(extStart, "GL_");
        if (!extStart)
            break;

        // find end of the name
        extEnd = extStart;
        while (*extEnd && *extEnd != ' ')
            extEnd++;

        // copy extension string name
        int extLen = extEnd - extStart;
        conf->extensionsList[count] = (char *) malloc(extLen + 1);
        memcpy(conf->extensionsList[count], extStart, extLen);
        conf->extensionsList[count][extLen] = 0;
        count++;

        // advance to next
        extStart = extEnd;
    }

    assert(count == conf->numExtensions);
}



static bool
matchKeyword(const char *string, const char *keyword)
{
    if (strncmp(string, keyword, strlen(keyword)) == 0)
        return true;
    else
        return false;
}


static char buffer[10*1000];

static char
getChar(FILE *f, const char **buf)
{
    if (feof(f))
        return 0;

    if (buf[0][0]) {
        char c = buf[0][0];
        (*buf)++;
        return c;
    }

    while (1) {
        // read another line
        *buf = fgets(buffer, sizeof(buffer), f);
        if (!buf || !buf[0]) {
            return 0;
        }
        else if (buf[0][0]) {
            char c = buf[0][0];
            (*buf)++;
            return c;
        }
        else if (feof(f)) {
            return 0;
        }
    }
}


// Scan 'buf' for a string value of the form: "string"  (double-quoted string)
// Return the string in newly malloc'd memory
static std::string
stringValue(FILE *f, const char *buf)
{
    std::string result;
    char c;

    // look for =
    do {
        c = getChar(f, &buf);
    } while (c && c != '=');

    if (!c)
        return result;

    // look for opening "
    do {
        c = getChar(f, &buf);
    } while (c && c != '\"');

    if (!c)
        return result;

    // scan characters in the string
    while (1) {
        c = getChar(f, &buf);
        if (!c) {
            std::cerr << "Error: didn't find closing \" in config file!\n";
            return result;
        }

        if (c == '\n') {
            // convert newlines inside a string into spaces
            c = ' ';
        }

        if (c == '\"') {
            // end of string
            return result;
        }
        else {
            result.push_back(c);
        }
    }
}


// Scan integer value of the form: 12345
static int
intValue(FILE *f, const char *buf)
{
    char valBuf[100];
    int valLen = 0;
    char c;

    // look for =
    do {
        c = getChar(f, &buf);
    } while (c && c != '=');

    if (!c)
        return 0;

    // scan digits
    while (1) {
        c = getChar(f, &buf);
        if (isdigit(c)) {
            valBuf[valLen++] = c;
        }
        else if (c != ' ') {
            break;
        }
    }

    valBuf[valLen] = 0;

    return atoi(valBuf);
}



static void
parse_file(FILE *f, configuration *conf)
{
    do {
        // read a line
        char *b = fgets(buffer, sizeof(buffer), f);

        if (!b) {
            break;
        }
        else if (b[0] == '#') {
            // comment - skip line
        }
        else if (b[0] == '\n') {
            // empty line
        }
        else if (matchKeyword(b, "GL_VENDOR")) {
            conf->vendor = stringValue(f, b + 10);
        }
        else if (matchKeyword(b, "GL_VERSION")) {
            conf->version = stringValue(f, b + 11);
        }
        else if (matchKeyword(b, "GL_EXTENSIONS")) {
            conf->extensions = stringValue(f, b + 14);
        }
        else if (matchKeyword(b, "GL_RENDERER")) {
            conf->renderer = stringValue(f, b + 12);
        }
        else if (matchKeyword(b, "GL_SHADING_LANGUAGE_VERSION")) {
            conf->glslVersion = stringValue(f, b + 28);
        }
        else if (matchKeyword(b, "GL_MAX_TEXTURE_SIZE")) {
            conf->maxTextureSize = intValue(f, b + 20);
        }
        else if (matchKeyword(b, "GL_MAJOR_VERSION")) {
            conf->versionMajor = intValue(f, b + 17);
        }
        else if (matchKeyword(b, "GL_MINOR_VERSION")) {
            conf->versionMinor = intValue(f, b + 17);
        }
        else if (matchKeyword(b, "GL_CONTEXT_PROFILE_MASK")) {
            std::string maskStr = stringValue(f, b + 24);
            conf->profileMask = 0x0;
            if (maskStr.find("GL_CONTEXT_CORE_PROFILE_BIT") != std::string::npos)
                conf->profileMask |= GL_CONTEXT_CORE_PROFILE_BIT;
            if (maskStr.find("GL_CONTEXT_COMPATIBILITY_PROFILE_BIT") != std::string::npos)
                conf->profileMask |= GL_CONTEXT_COMPATIBILITY_PROFILE_BIT;
        }
        else {
            std::cerr << "Unexpected config variable: " << b << ".\n";
            break;
        }
    } while (!feof(f));

    if (!conf->version.empty()) {
        // String version was specified, compute integer major/minor versions
        conf->versionMajor = conf->version[0] - '0';
        conf->versionMinor = conf->version[2] - '0';
        assert(conf->versionMajor >= 1 && conf->versionMajor <= 4);
        assert(conf->versionMinor >= 0 && conf->versionMinor <= 9);
    }
    else if (conf->versionMajor) {
        // Numeric version was specified, update the string
        if (!conf->version.empty()) {
            // if version string was specified too, override it
            conf->version[0] = '0' + conf->versionMajor;
            conf->version[2] = '0' + conf->versionMinor;
        }
        else {
            // allocate new version string
            conf->version.clear();
            conf->version.push_back('0' + conf->versionMajor);
            conf->version.push_back('.');
            conf->version.push_back('0' + conf->versionMinor);
        }
    }

    if (!conf->extensions.empty()) {
        create_extensions_list(conf);
    }
}


// Read the given configuration file and return a new configuration object.
// Return NULL if anything goes wrong
static configuration *
readConfigFile(const char *filename)
{
#ifndef NDEBUG
    os::log("apitrace: attempting to read configuration file: %s\n", filename);
#endif

    FILE *f = fopen(filename, "r");
    if (!f)
        return NULL;

    os::log("apitrace: using configuration file: %s\n", filename);

    configuration *conf = new configuration;

    parse_file(f, conf);

    fclose(f);

    if (1) {
        // debug
        os::log("apitrace: config GL_VENDOR = %s\n", conf->vendor.c_str());
        os::log("apitrace: config GL_VERSION = %s\n", conf->version.c_str());
        os::log("apitrace: config GL_EXTENSIONS = %s\n", conf->extensions.c_str());
        os::log("apitrace: config GL_NUM_EXTENSIONS = %d\n", conf->numExtensions);
        os::log("apitrace: config GL_RENDERER = %s\n", conf->renderer.c_str());
        os::log("apitrace: config GL_SHADING_LANGUAGE_VERSION = %s\n", conf->glslVersion.c_str());
        os::log("apitrace: config GL_MAX_TEXTURE_SIZE = %d\n", conf->maxTextureSize);
        os::log("apitrace: config GL_MAJOR_VERSION = %d\n", conf->versionMajor);
        os::log("apitrace: config GL_MINOR_VERSION = %d\n", conf->versionMinor);
        os::log("apitrace: config GL_CONTEXT_PROFILE_MASK = 0x%x\n", conf->profileMask);
    }

    return conf;
}


// Get pointer to configuration object or NULL if there was no config file.
const configuration *
getConfig(void)
{
    static bool configured = false;
    static configuration *config = NULL;

    if (!configured) {
        os::String configPath;
        const char *envConfigPath = getenv("GLTRACE_CONF");
        if (envConfigPath) {
            configPath = envConfigPath;
        } else {
            configPath = os::getConfigDir();
            configPath.join("apitrace");
            configPath.join("gltrace.conf");
        }
        config = gltrace::readConfigFile(configPath);
        configured = true;
    }

    return config;
}


// Return the named string value in the config object, or NULL.
const GLubyte *
getConfigString(const configuration *config, GLenum pname)
{
    if (!config)
        return NULL;

    switch (pname) {
    case GL_VERSION:
        return (const GLubyte *) config->version.c_str();
    case GL_VENDOR:
        return (const GLubyte *) config->vendor.c_str();
    case GL_EXTENSIONS:
        return (const GLubyte *) config->extensions.c_str();
    case GL_RENDERER:
        return (const GLubyte *) config->renderer.c_str();
    case GL_SHADING_LANGUAGE_VERSION:
        return (const GLubyte *) config->glslVersion.c_str();
    default:
        return NULL;
    }
}


// Return named indexed string value from config object, or NULL.
const GLubyte *
getConfigStringi(const configuration *config, GLenum pname, GLuint index)
{
    if (!config ||
        pname != GL_EXTENSIONS ||
        config->extensions.empty() ||
        index >= config->numExtensions) {
        return NULL;
    }

    return (const GLubyte *) config->extensionsList[index];
}



// Return named integer value from config object, or 0.
GLint
getConfigInteger(const configuration *config, GLenum pname)
{
    if (!config)
        return 0;

    switch (pname) {
    case GL_MAJOR_VERSION:
        return config->versionMajor;
    case GL_MINOR_VERSION:
        return config->versionMinor;
    case GL_CONTEXT_PROFILE_MASK:
        return config->profileMask;
    case GL_MAX_TEXTURE_SIZE:
        return config->maxTextureSize;
    case GL_NUM_EXTENSIONS:
        return config->numExtensions;
    default:
        return 0;
    }
}



} /* namespace gltrace */
