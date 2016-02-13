#include "os_osx.hpp"
#include "os_string.hpp"

#include <Foundation/Foundation.h>

namespace os {

String
getTemporaryDirectoryPath(void) {
    NSString *tempDir = NSTemporaryDirectory();
    const size_t len = strlen([tempDir fileSystemRepresentation]);
    char *string = (char *)calloc((sizeof(char) * len) + 1, 1);
    if (![tempDir getFileSystemRepresentation:string maxLength:len + 1]) {
        NSLog(@"apitrace: error: cant get temporary directory");
    }
    return String(string);
}
}
