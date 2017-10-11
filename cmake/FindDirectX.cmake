# - try to find DirectX include directories and libraries
#
# Once done this will define:
#
#  DirectX_XYZ_INCLUDE_FOUND - system has the include for the XYZ API
#  DirectX_XYZ_INCLUDE_DIR   - include directory for the XYZ API
#
# Where XYZ can be any of:
#
#  DDRAW
#  D3D
#  D3D8
#  D3D9
#  D3D10
#  D3D10_1
#  D3D11
#  D3D11_1
#  D3D11_2
#  D2D1
#


include (CheckIncludeFileCXX)
include (FindPackageMessage)


if (WIN32)

    if (CMAKE_SIZEOF_VOID_P EQUAL 8)
        set (DirectX_ARCHITECTURE x64)
    else ()
        set (DirectX_ARCHITECTURE x86)
    endif ()

    # Can't use "$ENV{ProgramFiles(x86)}" to avoid violating CMP0053.  See
    # http://public.kitware.com/pipermail/cmake-developers/2014-October/023190.html
    set (ProgramFiles_x86 "ProgramFiles(x86)")
    if ("$ENV{${ProgramFiles_x86}}")
        set (ProgramFiles "$ENV{${ProgramFiles_x86}}")
    else ()
        set (ProgramFiles "$ENV{ProgramFiles}")
    endif ()

    find_path (DirectX_ROOT_DIR
        Include/d3d9.h
        PATHS
            "$ENV{DXSDK_DIR}"
            "${ProgramFiles}/Microsoft DirectX SDK (June 2010)"
            "${ProgramFiles}/Microsoft DirectX SDK (February 2010)"
            "${ProgramFiles}/Microsoft DirectX SDK (March 2009)"
            "${ProgramFiles}/Microsoft DirectX SDK (August 2008)"
            "${ProgramFiles}/Microsoft DirectX SDK (June 2008)"
            "${ProgramFiles}/Microsoft DirectX SDK (March 2008)"
            "${ProgramFiles}/Microsoft DirectX SDK (November 2007)"
            "${ProgramFiles}/Microsoft DirectX SDK (August 2007)"
            "${ProgramFiles}/Microsoft DirectX SDK"
        DOC "DirectX SDK root directory"
    )
    if (DirectX_ROOT_DIR)
        set (DirectX_INC_SEARCH_PATH "${DirectX_ROOT_DIR}/Include")
        set (DirectX_LIB_SEARCH_PATH "${DirectX_ROOT_DIR}/Lib/${DirectX_ARCHITECTURE}")
        set (DirectX_BIN_SEARCH_PATH "${DirectX_ROOT_DIR}/Utilities/bin/x86")
    endif ()

    # With VS 2011 and Windows 8 SDK, the DirectX SDK is included as part of
    # the Windows SDK.
    #
    # See also:
    # - http://msdn.microsoft.com/en-us/library/windows/desktop/ee663275.aspx
    if (MSVC)
        set (USE_WINSDK_HEADERS TRUE)
    endif ()

    # Find a header in the DirectX SDK
    macro (find_dxsdk_header var_name header)
        set (include_dir_var "DirectX_${var_name}_INCLUDE_DIR")
        set (include_found_var "DirectX_${var_name}_INCLUDE_FOUND")
        find_path (${include_dir_var} ${header}
            HINTS ${DirectX_INC_SEARCH_PATH}
            DOC "The directory where ${header} resides"
            CMAKE_FIND_ROOT_PATH_BOTH
        )
        if (${include_dir_var})
            set (${include_found_var} TRUE)
            find_package_message (${var_name}_INC "Found ${header} header: ${${include_dir_var}}/${header}" "[${${include_dir_var}}]")
        endif ()
        mark_as_advanced (${include_found_var})
    endmacro ()

    # Find a header in the Windows SDK
    macro (find_winsdk_header var_name header)
        if (USE_WINSDK_HEADERS)
            # Windows SDK
            set (include_dir_var "DirectX_${var_name}_INCLUDE_DIR")
            set (include_found_var "DirectX_${var_name}_INCLUDE_FOUND")
            check_include_file_cxx (${header} ${include_found_var})
            set (${include_dir_var})
            mark_as_advanced (${include_found_var})
        else ()
            find_dxsdk_header (${var_name} ${header})
        endif ()
    endmacro ()

    find_winsdk_header  (D3D     d3d.h)

    if (MSVC)
        set (DirectX_D3D8_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/thirdparty/dxsdk_aug2007)
        set (DirectX_D3D8_INCLUDE_FOUND TRUE)
        mark_as_advanced (DirectX_D3D8_INCLUDE_FOUND)
    else ()
        find_dxsdk_header   (D3D8    d3d8.h)
    endif ()

    find_winsdk_header  (D3D9    d3d9.h)

    find_winsdk_header  (D3D11_4 d3d11_4.h)

    find_winsdk_header  (D2D1_1  d2d1_1.h)

    find_program (DirectX_FXC_EXECUTABLE fxc
        HINTS ${DirectX_BIN_SEARCH_PATH}
        DOC "Path to fxc.exe executable."
    )

endif ()
