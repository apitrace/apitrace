# CMake will throw a cryptic message if an user unwittingly invokes it from
# this directory.
if (CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    message (FATAL_ERROR
        "qapitrace must be built from the top source directory.\n"
        "Read docs/INSTALL.markdown for detailed build instructions."
    )
endif ()

add_definitions (-DQT_FORCE_ASSERTS)

if (APPLE)
    # Recent builds of Qt no longer support i386 architecture
    set (CMAKE_OSX_ARCHITECTURES "x86_64")
endif()

add_library (qubjson STATIC
    qubjson.cpp
)

add_gtest (qubjson_test qubjson_test.cpp)
target_link_libraries (qubjson_test qubjson)

set(qapitrace_SRCS
   androiddevicedialog.cpp
   androidfiledialog.cpp
   androidretracer.cpp
   androidutils.cpp
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

qt5_add_resources(qapitrace_SRCS qapitrace.qrc)

set(qapitrace_UIS
   ui/androidfiledialog.ui
   ui/androiddevicedialog.ui
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

set (CMAKE_AUTOMOC ON)
# Silence `Note: No relevant classes found. No output generated.`
set (CMAKE_AUTOMOC_MOC_OPTIONS "-nn")

QT5_WRAP_UI(qapitrace_UIS_H ${qapitrace_UIS})
set(CMAKE_INCLUDE_CURRENT_DIR ON)

#add_app_icon(qapitrace_SRCS ../icons/hi*-qapitrace.png)
include_directories(
    ${CMAKE_SOURCE_DIR} # for image/*
)

add_executable (qapitrace ${qapitrace_SRCS} ${qapitrace_UIS_H})

# Avoid qt5_use_modules. See also:
# - https://bugreports.qt.io/browse/QTBUG-39457
# - http://doc.qt.io/qt-5/cmake-manual.html
# - http://www.kdab.com/using-cmake-with-qt-5/
if (Qt5Core_VERSION_STRING VERSION_LESS 5.2.0)
    qt5_use_modules (qubjson Widgets)
    qt5_use_modules (qubjson_test Widgets)
    qt5_use_modules (qapitrace Widgets WebKitWidgets)
else ()
    target_link_libraries (qubjson Qt5::Widgets)
    target_link_libraries (qubjson_test Qt5::Widgets)
    target_link_libraries (qapitrace Qt5::Widgets Qt5::WebKitWidgets)
endif ()

target_link_libraries (qapitrace
    qubjson
    image
    common
    ${ZLIB_LIBRARIES}
    ${SNAPPY_LIBRARIES}
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
