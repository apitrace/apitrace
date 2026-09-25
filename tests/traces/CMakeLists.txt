if (APITRACE_EXECUTABLE AND APITRACE_SOURCE_DIR)
    file (GLOB TEST_SCRIPTS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.script)
    list (SORT TEST_SCRIPTS)
    foreach (TEST_SCRIPT ${TEST_SCRIPTS})
        get_filename_component (TEST_NAME ${TEST_SCRIPT} NAME_WE)
        add_test(
            NAME ${TEST_NAME}
            COMMAND
            ${Python3_EXECUTABLE} ${PROJECT_SOURCE_DIR}/tool_driver.py
                --apitrace ${APITRACE_EXECUTABLE}
                --apitrace-source ${APITRACE_SOURCE_DIR}
                ${CMAKE_CURRENT_SOURCE_DIR}/${TEST_SCRIPT}
        )
    endforeach ()
endif ()
