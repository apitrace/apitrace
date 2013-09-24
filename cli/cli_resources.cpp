/*********************************************************************
 *
 * Copyright 2011 Intel Corporation
 * All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 *********************************************************************/


#include <iostream>

#include "os_string.hpp"

#include "cli_resources.hpp"

static bool
tryPath(const os::String &path, bool verbose)
{
    bool exists = path.exists();
    if (verbose) {
        std::cerr << "info: " << (exists ? "found" : "did not find") << " " << path.str() << "\n";
    }
    return exists;
}

#ifdef APITRACE_CONFIG_INSTALL_DIR
#include <fstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <errno.h>

/* TODO: This should either be a C++11 TR 2-style directory iterator
 * implementation or should just use libboost-filesystem.
 */

class DirectoryListing {
public:
    DirectoryListing(os::String path);
    ~DirectoryListing();

    os::String next();

private:
    const os::String basepath;
    struct dirent *entry;
    DIR *dir;
    int error;
};

DirectoryListing::DirectoryListing(os::String path) : basepath(path)
{
    error = 0;

    int len = offsetof(struct dirent, d_name) + pathconf(path, _PC_NAME_MAX) + 1;
    entry = static_cast<struct dirent *>(::operator new(len));
    dir = opendir(path);

    if (dir == NULL)
        error = errno;
}

DirectoryListing::~DirectoryListing()
{
    closedir(dir);
    ::operator delete(entry);
}

os::String DirectoryListing::next(void)
{
    struct dirent *result;

    if (error)
        return os::String();

    error = readdir_r(dir, entry, &result);
    if (result == NULL)
        return os::String();
    if (!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, ".."))
        return next();

    os::String filename(basepath);
    filename.join(entry->d_name);

    return filename;
}

os::String
findAllWrappers(const char *wrapperFilename, bool verbose)
{
    os::String conffilePath;
    os::String wrapperPaths;
    DirectoryListing conffileListing(APITRACE_CONFIG_INSTALL_DIR);

    os::String wrapperPath = findWrapper(wrapperFilename, verbose);
    if (wrapperPath.length()) {
        wrapperPath.trimFilename();
        wrapperPaths.append(wrapperPath);
        wrapperPaths.append(":");
    }

    if (verbose)
        std::cerr << "info: reading conf files in: " << APITRACE_CONFIG_INSTALL_DIR << std::endl;

    conffilePath = conffileListing.next();
    while (conffilePath.length() > 0) {
        if (verbose)
            std::cerr << "info: reading paths from conf file:" << conffilePath.str() << std::endl;
        std::ifstream conffile(conffilePath);
        while(conffile.good()) {
            char path[PATH_MAX];
            bool exists;
            conffile.getline(path, PATH_MAX);
            wrapperPath = path;

            wrapperPath.join(wrapperFilename);
            exists = wrapperPath.exists();

            if (tryPath(wrapperPath, verbose)) {
                wrapperPaths.append(path);
                wrapperPaths.append(":");
            }
        }
        conffilePath = conffileListing.next();
    }

    /* Trim off trailing ':' */
    if (wrapperPaths.length() > 0)
        wrapperPaths.truncate(wrapperPaths.length() - 1);
    
    return wrapperPaths;
}

#endif /* APITRACE_CONFIG_INSTALL_DIR */

os::String
findProgram(const char *programFilename, bool verbose)
{
    os::String programPath;

    os::String processDir = os::getProcessName();
    processDir.trimFilename();

    programPath = processDir;
    programPath.join(programFilename);
    if (tryPath(programPath, verbose)) {
        return programPath;
    }

#ifndef _WIN32
    // Try absolute install directory
    programPath = APITRACE_PROGRAMS_INSTALL_DIR;
    programPath.join(programFilename);
    if (tryPath(programPath, verbose)) {
        return programPath;
    }
#endif

    return "";
}

os::String
findWrapper(const char *wrapperFilename, bool verbose)
{
    os::String wrapperPath;

    os::String processDir = os::getProcessName();
    processDir.trimFilename();

    // Try relative build directory
    // XXX: Just make build and install directory layout match
    wrapperPath = processDir;
#if defined(APITRACE_CONFIGURATION_SUBDIR)
    // Go from `Debug\apitrace.exe` to `wrappers\Debug\foo.dll` on MSVC builds.
    wrapperPath.join("..");
    wrapperPath.join("wrappers");
    wrapperPath.join(APITRACE_CONFIGURATION_SUBDIR);
#else
    wrapperPath.join("wrappers");
#endif
    wrapperPath.join(wrapperFilename);
    if (tryPath(wrapperPath, verbose)) {
        return wrapperPath;
    }

    // Try relative install directory
    wrapperPath = processDir;
#if defined(_WIN32)
    wrapperPath.join("..\\lib\\wrappers");
#elif defined(__APPLE__)
    wrapperPath.join("../lib/wrappers");
#else
    wrapperPath.join("../lib/apitrace/wrappers");
#endif
    wrapperPath.join(wrapperFilename);
    if (tryPath(wrapperPath, verbose)) {
        return wrapperPath;
    }

#ifndef _WIN32
    // Try absolute install directory
    wrapperPath = APITRACE_WRAPPERS_INSTALL_DIR;
    wrapperPath.join(wrapperFilename);
    if (tryPath(wrapperPath, verbose)) {
        return wrapperPath;
    }
#endif

    return "";
}

os::String
findScript(const char *scriptFilename, bool verbose)
{
    os::String scriptPath;

    os::String processDir = os::getProcessName();
    processDir.trimFilename();

    // Try relative build directory
    // XXX: Just make build and install directory layout match
#if defined(APITRACE_SOURCE_DIR)
    scriptPath = APITRACE_SOURCE_DIR;
    scriptPath.join("scripts");
    scriptPath.join(scriptFilename);
    if (tryPath(scriptPath, verbose)) {
        return scriptPath;
    }
#endif

    // Try relative install directory
    scriptPath = processDir;
#if defined(_WIN32)
    scriptPath.join("..\\lib\\scripts");
#elif defined(__APPLE__)
    scriptPath.join("../lib/scripts");
#else
    scriptPath.join("../lib/apitrace/scripts");
#endif
    scriptPath.join(scriptFilename);
    if (tryPath(scriptPath, verbose)) {
        return scriptPath;
    }

#ifndef _WIN32
    // Try absolute install directory
    scriptPath = APITRACE_SCRIPTS_INSTALL_DIR;
    scriptPath.join(scriptFilename);
    if (tryPath(scriptPath, verbose)) {
        return scriptPath;
    }
#endif

    std::cerr << "error: cannot find " << scriptFilename << " script\n";

    return "";
}
