set(qapitrace_SRCS
   apitracecall.cpp
   apitracefilter.cpp
   apitracemodel.cpp
   loaderthread.cpp
   mainwindow.cpp
   main.cpp
 )

qt4_automoc(${qapitrace_SRCS})

set(qapitrace_UIS
   ui/mainwindow.ui)

QT4_WRAP_UI(qapitrace_UIS_H ${qapitrace_UIS})

#add_app_icon(qapitrace_SRCS ../icons/hi*-qapitrace.png)

include_directories(${QT_INCLUDES} ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/..)

add_executable(qapitrace ${qapitrace_SRCS} ${qapitrace_UIS_H})

target_link_libraries(qapitrace trace ${QT_QTCORE_LIBRARIES} ${QT_QTGUI_LIBRARIES} )

########### install files ###############

#install(TARGETS qapitrace ${INSTALL_TARGETS_DEFAULT_ARGS} )
#install( FILES qapitrace.desktop  DESTINATION ${XDG_APPS_INSTALL_DIR} )
