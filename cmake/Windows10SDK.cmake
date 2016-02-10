# - try to find and use Windows 10 SDK on VS 2013
#
# VS 2013 will never pick Windows 10 SDK -- one must manually set the include and library directories.
#
# See also:
# - https://github.com/walbourn/directx-sdk-samples/commit/68575c40984f2581e3931157bd7e9f831ba70d69
# - http://stackoverflow.com/a/28922582
# - http://blogs.msdn.com/b/vcblog/archive/2012/11/23/using-the-windows-8-sdk-with-visual-studio-2010-configuring-multiple-projects.aspx

if (MSVC AND ${MSVC_VERSION} LESS 1900 AND NOT CMAKE_GENERATOR_TOOLSET MATCHES "_xp$")

    include (FindPackageMessage)

    # Can't use "$ENV{ProgramFiles(x86)}" to avoid violating CMP0053.  See
    # http://public.kitware.com/pipermail/cmake-developers/2014-October/023190.html
    set (ProgramFiles_x86 "ProgramFiles(x86)")
    if ("$ENV{${ProgramFiles_x86}}")
        set (ProgramFiles "$ENV{${ProgramFiles_x86}}")
    else ()
        set (ProgramFiles "$ENV{ProgramFiles}")
    endif ()

    set (WIN10_SDK_VER "10.0.10240.0")

    find_path (WIN10_SDK_ROOT_DIR
        SDKManifest.xml
        PATHS
            "${ProgramFiles}/Windows Kits/10"
            DOC "Windows 10 SDK root directory"
    )

    if (CMAKE_SIZEOF_VOID_P EQUAL 4)
        set (WIN10_SDK_ARCH x86)
    else ()
        set (WIN10_SDK_ARCH x64)
    endif ()

    if (WIN10_SDK_ROOT_DIR)
        find_package_message (WIN10_SDK "Windows 10 SDK - found: ${WIN10_SDK_ROOT_DIR}" "[${WIN10_SDK_ROOT_DIR}]")
        set (WIN10_SDK_INC_DIRS
            "${WIN10_SDK_ROOT_DIR}/Include/${WIN10_SDK_VER}/um"
            "${WIN10_SDK_ROOT_DIR}/Include/${WIN10_SDK_VER}/shared"
            "${WIN10_SDK_ROOT_DIR}/Include/${WIN10_SDK_VER}/winrt"
        )
        include_directories (BEFORE ${WIN10_SDK_INC_DIRS})
        list (APPEND CMAKE_REQUIRED_INCLUDES ${WIN10_SDK_INC_DIRS})
        link_directories (
            ${WIN10_SDK_ROOT_DIR}/Lib/${WIN10_SDK_VER}/${WIN10_SDK_ARCH}
        )
    else ()
        find_package_message (WIN10_SDK "Windows 10 SDK - not found" "[]")
    endif ()
endif ()
