# CMake will throw a cryptic message if an user unwittingly invokes it from
# this directory.
if (CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    message (FATAL_ERROR
        "qapitrace must be built from the top source directory.\n"
        "Read docs/INSTALL.markdown for detailed build instructions."
    )
endif ()

add_definitions (-DQT_FORCE_ASSERTS -DQT_DISABLE_DEPRECATED_BEFORE=0x050F00)

add_library (qubjson STATIC
    qubjson.cpp
)
target_link_libraries (qubjson Qt::Widgets)

if (BUILD_TESTING)
    add_gtest (qubjson_test qubjson_test.cpp)
    target_link_libraries (qubjson_test qubjson)
endif ()

set(qapitrace_SRCS
   adaptivedoublespinbox.cpp
   apisurface.cpp
   apitrace.cpp
   apicalldelegate.cpp
   apitracecall.cpp
   apitracefilter.cpp
   apitracemodel.cpp
   argumentseditor.cpp
   glsledit.cpp
   imageviewer.cpp
   jumpwidget.cpp
   mainwindow.cpp
   main.cpp
   pixelwidget.cpp
   profiledialog.cpp
   profiletablemodel.cpp
   retracer.cpp
   saverthread.cpp
   searchwidget.cpp
   settingsdialog.cpp
   shaderssourcewidget.cpp
   tracedialog.cpp
   traceloader.cpp
   traceprocess.cpp
   trimprocess.cpp
   vertexdatainterpreter.cpp
   leaktracethread.cpp
   graphing/frameaxiswidget.cpp
   graphing/graphwidget.cpp
   graphing/graphaxiswidget.cpp
   graphing/graphview.cpp
   graphing/heatmapview.cpp
   graphing/heatmapverticalaxiswidget.cpp
   graphing/histogramview.cpp
   graphing/timeaxiswidget.cpp
 )

qt_add_resources(qapitrace_SRCS qapitrace.qrc)

set(qapitrace_UIS
   ui/argumentseditor.ui
   ui/imageviewer.ui
   ui/jumpwidget.ui
   ui/mainwindow.ui
   ui/profiledialog.ui
   ui/profilereplaydialog.ui
   ui/retracerdialog.ui
   ui/settings.ui
   ui/tracedialog.ui
   ui/searchwidget.ui
   ui/shaderssourcewidget.ui
   )

# https://cmake.org/cmake/help/v3.11/policy/CMP0071.html
if (POLICY CMP0071)
    cmake_policy (SET CMP0071 NEW)
endif()

set (CMAKE_AUTOMOC ON)

# Silence `Note: No relevant classes found. No output generated.`
set (CMAKE_AUTOMOC_MOC_OPTIONS "-nn")

QT_WRAP_UI (qapitrace_UIS_H ${qapitrace_UIS})
set (CMAKE_INCLUDE_CURRENT_DIR ON)

#add_app_icon(qapitrace_SRCS ../icons/hi*-qapitrace.png)
include_directories (
    ${CMAKE_SOURCE_DIR}/lib/image
    ${CMAKE_SOURCE_DIR}/lib/ubjson
    ${CMAKE_BINARY_DIR}
)

add_dependencies(apitrace version)

add_executable (qapitrace ${qapitrace_SRCS} ${qapitrace_UIS_H})

target_link_libraries (qapitrace
    Qt::Widgets
    qubjson
    image
    common
    ${CMAKE_THREAD_LIBS_INIT}
)

if (WIN32 AND NOT (CMAKE_BUILD_TYPE STREQUAL "Debug"))
    # Use Windows subsystem (i.e., no console).
    set_target_properties (qapitrace PROPERTIES CMAKE_WIN32_EXECUTABLE TRUE)
    if (MSVC)
        # When the Windows subsystem is chosen by default MSVC expects WinMain()
        # entry point, but we rather use plain old main() everywhere.
        set_target_properties (qapitrace PROPERTIES LINK_FLAGS "/ENTRY:mainCRTStartup")
    endif ()
endif ()


########### install files ###############

install (TARGETS qapitrace RUNTIME DESTINATION bin)
install_pdb (qapitrace RUNTIME DESTINATION bin)
#install (FILES qapitrace.desktop DESTINATION ${XDG_APPS_INSTALL_DIR})

# Deployt Qt dependencies
#
# See:
# - http://doc.qt.io/qt-5/windows-deployment.html
#
# XXX: windeployqt can fail if there's weird stuff in the Path env var, so we
# invoke it with a sanitized Path, via `cmake -E env` that was introduced in
# CMake 3.1.
if (WIN32 AND NOT CMAKE_CROSSCOMPILING)
    # http://stackoverflow.com/a/24676432
    get_target_property (QT_QMAKE_EXECUTABLE Qt::qmake IMPORTED_LOCATION)
    get_filename_component (QT_BIN_DIR "${QT_QMAKE_EXECUTABLE}" DIRECTORY)
    find_program (QT_WINDEPLOYQT_EXECUTABLE windeployqt HINTS "${QT_BIN_DIR}")
    if (QT_WINDEPLOYQT_EXECUTABLE)
        file (TO_NATIVE_PATH "${QT_BIN_DIR}" QT_BIN_DIR_NATIVE)
        # It's safer to use `\` separateors in the Path, but we need to escape them
        string (REPLACE "\\" "\\\\" QT_BIN_DIR_NATIVE "${QT_BIN_DIR_NATIVE}")

        # windeployqt needs VCINSTALLDIR to copy MSVC Runtime files, but it's
        # usually not define with MSBuild builds.
        if (DEFINED ENV{VCINSTALLDIR})
            set (VCINSTALLDIR "$ENV{VCINSTALLDIR}" CACHE PATH "VC installation directory")
        elseif (MSVC14)
            # https://developercommunity.visualstudio.com/solutions/21956/view.html
            get_filename_component (VSINSTALLDIR [HKEY_LOCAL_MACHINE\\SOFTWARE\\WOW6432Node\\Microsoft\\VisualStudio\\SxS\\VS7;15.0] REALPATH CACHE)
            if (IS_DIRECTORY "${VSINSTALLDIR}")
                set (VCINSTALLDIR "${VSINSTALLDIR}/VC" CACHE PATH "VC installation directory")
            endif ()
        else ()
            message (FATAL_ERROR "Unsupported MSVC version ${MSVC_VERSION}")
        endif ()
        message (STATUS "VCINSTALLDIR: ${VCINSTALLDIR}")

        install (CODE "
            message (STATUS \"Running Qt Deploy Tool...\")
            if (CMAKE_INSTALL_CONFIG_NAME STREQUAL \"Debug\")
                list (APPEND QT_WINDEPLOYQT_FLAGS --debug)
            else ()
                list (APPEND QT_WINDEPLOYQT_FLAGS --release)
            endif ()
            execute_process (COMMAND
                \"${CMAKE_COMMAND}\" -E env
                \"Path=${QT_BIN_DIR_NATIVE};\$ENV{SystemRoot}\\\\System32;\$ENV{SystemRoot}\"
                \"VCINSTALLDIR=${VCINSTALLDIR}\"
                \"${QT_WINDEPLOYQT_EXECUTABLE}\"
                \${QT_WINDEPLOYQT_FLAGS}
                \"\${CMAKE_INSTALL_PREFIX}/bin/qapitrace.exe\"
            )
        ")
    endif ()
endif ()
