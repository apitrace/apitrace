# - try to find DirectX include directories and libraries
#
# Once done this will define:
#
#  DirectX_XYZ_FOUND         - system has the XYZ API
#  DirectX_XYZ_INCLUDE_FOUND - system has the include for the XYZ API
#  DirectX_XYZ_INCLUDE_DIR   - include directory for the XYZ API
#  DirectX_XYZ_LIBRARY       - path/name for the XYZ library
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
        Include/d3d8.h
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

    # Find a library in the DirectX SDK
    macro (find_dxsdk_library var_name library)
        # DirectX SDK
        set (library_var "DirectX_${var_name}_LIBRARY")
        find_library (${library_var} ${library}
            HINTS ${DirectX_LIB_SEARCH_PATH}
            DOC "The directory where ${library} resides"
            CMAKE_FIND_ROOT_PATH_BOTH
        )
        if (${library_var})
            find_package_message (${var_name}_LIB "Found ${library} library: ${${library_var}}" "[${${library_var}}]")
        endif ()
        mark_as_advanced (${library_var})
    endmacro ()

    # Combine header and library variables into an API found variable
    macro (find_combined var_name inc_var_name lib_var_name)
        if (DirectX_${inc_var_name}_INCLUDE_FOUND AND DirectX_${lib_var_name}_LIBRARY)
            set (DirectX_${var_name}_FOUND 1)
            find_package_message (${var_name} "Found ${var_name} API" "[${DirectX_${lib_var_name}_LIBRARY}][${DirectX_${inc_var_name}_INCLUDE_DIR}]")
        endif ()
    endmacro ()

    if (MINGW)
        # MinGW always has D3D8 headers and library.  They are not perfect, but
        # sufficient for our test apps.
        set (DirectX_D3D8_INCLUDE_DIR)
        set (DirectX_D3D8_LIBRARY d3d8)
        set (DirectX_D3D8_FOUND TRUE)
        mark_as_advanced (DirectX_D3D8_FOUND)
    else ()
        find_dxsdk_header   (D3D8    d3d8.h)
        find_dxsdk_library  (D3D8    d3d8)
        find_combined       (D3D8    D3D8 D3D8)
    endif ()

    find_program (DirectX_FXC_EXECUTABLE fxc
        HINTS ${DirectX_BIN_SEARCH_PATH}
        DOC "Path to fxc.exe executable."
    )

    # MinGW lacks libd3d10.a and libd3d10_1.a
    if (MINGW)

        # Find dlltool
        if (CMAKE_VERSION VERSION_LESS 3.16)
            get_filename_component (GCC_NAME ${CMAKE_C_COMPILER} NAME)
            string (REGEX REPLACE [[gcc(-(posix|win32))?]] dlltool DLLTOOL_NAME ${GCC_NAME})
            find_program (CMAKE_DLLTOOL NAMES ${DLLTOOL_NAME})
            if (CMAKE_DLLTOOL)
                message (STATUS "Found dlltool: ${CMAKE_DLLTOOL}")
            else ()
                message (SEND_ERROR "dlltool not found")
            endif ()
        endif ()

        macro (add_dxsdk_implib target def)
            set (MGWHELP_IMPLIB ${CMAKE_BINARY_DIR}/lib${target}.a)
            add_custom_command (
                OUTPUT ${MGWHELP_IMPLIB}
                COMMAND
                    ${CMAKE_DLLTOOL}
                    --output-lib ${MGWHELP_IMPLIB}
                    --dllname ${target}.dll
                    --kill-at
                    --input-def=${CMAKE_CURRENT_LIST_DIR}/${def}
                DEPENDS ${CMAKE_CURRENT_LIST_DIR}/${def}
            )
            add_custom_target (lib${target} DEPENDS ${MGWHELP_IMPLIB})
            add_library (${target} INTERFACE IMPORTED GLOBAL)
            target_link_libraries (${target} INTERFACE ${MGWHELP_IMPLIB})
        endmacro ()

        if (CMAKE_SIZEOF_VOID_P EQUAL 4)
            add_dxsdk_implib (d3d10 d3d10.x86.def)
            add_dxsdk_implib (d3d10_1 d3d10_1.x86.def)
        else ()
            add_dxsdk_implib (d3d10 d3d10.x64.def)
            add_dxsdk_implib (d3d10_1 d3d10_1.x64.def)
        endif ()

    endif ()

endif ()
