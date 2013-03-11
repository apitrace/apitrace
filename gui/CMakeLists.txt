# Include the cmake file needed to use qt4
include( ${QT_USE_FILE} )

set(qapitrace_SRCS
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
   graphing/frameaxiswidget.cpp
   graphing/graphwidget.cpp
   graphing/graphaxiswidget.cpp
   graphing/graphview.cpp
   graphing/heatmapview.cpp
   graphing/heatmapverticalaxiswidget.cpp
   graphing/histogramview.cpp
   graphing/timeaxiswidget.cpp
 )

qt4_automoc(${qapitrace_SRCS})

qt4_add_resources(qapitrace_SRCS qapitrace.qrc)

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

QT4_WRAP_UI(qapitrace_UIS_H ${qapitrace_UIS})

#add_app_icon(qapitrace_SRCS ../icons/hi*-qapitrace.png)
link_directories(${LINK_DIRECTORIES} ${QJSON_LIBRARY_DIRS})
include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}
    ${QJSON_INCLUDE_DIR}
    ${QT_INCLUDES}
)

if (WIN32)
    # Use Windows subsystem (i.e., no console).
    set (qapitrace_SUBSYSTEM "WIN32")
endif ()

add_executable(qapitrace ${qapitrace_SUBSYSTEM} ${qapitrace_SRCS} ${qapitrace_UIS_H})

target_link_libraries (qapitrace
    image
    common
    ${ZLIB_LIBRARIES}
    ${SNAPPY_LIBRARIES}
    ${QJSON_LIBRARIES}
    ${QT_LIBRARIES}
    ${CMAKE_THREAD_LIBS_INIT}
)

if (APPLE)
    # Recent builds of Qt no longer support i386 architecture
    set_target_properties (qapitrace PROPERTIES OSX_ARCHITECTURES x86_64)
endif ()
if (MSVC)
    # When the Windows subsystem is chosen by default MSVC expects WinMain()
    # entry point, but we rather use plain old main() everywhere.
    set_target_properties (qapitrace PROPERTIES LINK_FLAGS "/ENTRY:mainCRTStartup")
endif ()


########### install files ###############

install (TARGETS qapitrace RUNTIME DESTINATION bin)
#install (FILES qapitrace.desktop DESTINATION ${XDG_APPS_INSTALL_DIR})
