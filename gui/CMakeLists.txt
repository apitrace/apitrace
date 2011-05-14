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
   loaderthread.cpp
   mainwindow.cpp
   main.cpp
   retracer.cpp
   saverthread.cpp
   searchwidget.cpp
   settingsdialog.cpp
   shaderssourcewidget.cpp
   tracedialog.cpp
   traceprocess.cpp
   vertexdatainterpreter.cpp
 )

qt4_automoc(${qapitrace_SRCS})

qt4_add_resources(qapitrace_SRCS qapitrace.qrc)

set(qapitrace_UIS
   ui/argumentseditor.ui
   ui/imageviewer.ui
   ui/jumpwidget.ui
   ui/mainwindow.ui
   ui/retracerdialog.ui
   ui/settings.ui
   ui/tracedialog.ui
   ui/searchwidget.ui
   ui/shaderssourcewidget.ui
   )

QT4_WRAP_UI(qapitrace_UIS_H ${qapitrace_UIS})

#add_app_icon(qapitrace_SRCS ../icons/hi*-qapitrace.png)
link_directories(${LINK_DIRECTORIES} ${QJSON_LIBRARY_DIRS})
include_directories(${QT_INCLUDES} ${QJSON_INCLUDE_DIR} ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/..)

add_definitions(-DBUILD_DIR="${CMAKE_BINARY_DIR}")

add_executable(qapitrace ${qapitrace_SRCS} ${qapitrace_UIS_H})

target_link_libraries(qapitrace trace ${QJSON_LIBRARIES} ${QT_LIBRARIES} )

########### install files ###############

install (TARGETS qapitrace RUNTIME DESTINATION bin)
#install (FILES qapitrace.desktop DESTINATION ${XDG_APPS_INSTALL_DIR})
