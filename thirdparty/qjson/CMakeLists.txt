add_definitions (
    -DQT_NO_CAST_FROM_ASCII
    -DQT_NO_CAST_TO_ASCII
    -DQT_NO_DEBUG_OUTPUT
)

set (QT_DONT_USE_QTGUI TRUE)

include (${QT_USE_FILE})

include_directories (${CMAKE_CURRENT_BINARY_DIR})

set (qjson_MOC_HDRS
    parserrunnable.h
    #serializerrunnable.h
)

qt4_wrap_cpp (qjson_MOC_SRCS ${qjson_MOC_HDRS})

set (qjson_SRCS
    parser.cpp
    qobjecthelper.cpp
    json_scanner.cpp
    json_parser.cc
    parserrunnable.cpp
    #serializer.cpp
    #serializerrunnable.cpp
)

add_library (qjson_bundled STATIC EXCLUDE_FROM_ALL
    ${qjson_SRCS}
    ${qjson_MOC_SRCS}
)

install (
    FILES COPYING.lib
    DESTINATION ${DOC_INSTALL_DIR}
    RENAME LICENSE-qjson.txt
)
