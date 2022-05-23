install (
    PROGRAMS
        convert.py
        jsondiff.py
        jsonextractimages.py
        leaks.py
        profileshader.py
        retracediff.py
        snapdiff.py
        tracecheck.py
        tracediff.py
        unpickle.py
    DESTINATION ${SCRIPTS_INSTALL_DIR}
)
install (
    FILES
        apitrace.PIXExp
        highlight.py
    DESTINATION ${SCRIPTS_INSTALL_DIR}
)
install (
    FILES apitrace.PIXExp
    DESTINATION ${SCRIPTS_INSTALL_DIR}
)
