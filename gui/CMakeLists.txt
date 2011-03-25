cmake_minimum_required(VERSION 2.8)

find_package(Qt4 REQUIRED)

set(qapitrace_SRCS
   ../trace_model.cpp
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

if (ZLIB_FOUND)
    include_directories (${ZLIB_INCLUDE_DIRS})
    link_libraries (${ZLIB_LIBRARIES})
else (ZLIB_FOUND)
    add_library (zlib STATIC
        ../zlib/adler32.c
        ../zlib/compress.c
        ../zlib/crc32.c
        ../zlib/gzio.c
        ../zlib/uncompr.c
        ../zlib/deflate.c
        ../zlib/trees.c
        ../zlib/zutil.c
        ../zlib/inflate.c
        ../zlib/infback.c
        ../zlib/inftrees.c
        ../zlib/inffast.c
    )
    include_directories (zlib)
    link_libraries (zlib)
endif (ZLIB_FOUND)

include_directories(${QT_INCLUDES} ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/..)

add_executable(qapitrace ${qapitrace_SRCS} ${qapitrace_UIS_H})

target_link_libraries(qapitrace ${QT_QTCORE_LIBRARIES} ${QT_QTGUI_LIBRARIES} )

########### install files ###############

#install(TARGETS qapitrace ${INSTALL_TARGETS_DEFAULT_ARGS} )
#install( FILES qapitrace.desktop  DESTINATION ${XDG_APPS_INSTALL_DIR} )
