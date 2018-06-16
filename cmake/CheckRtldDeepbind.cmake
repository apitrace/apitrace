include(CheckCSourceCompiles)

macro(apitrace_check_rtld_deepbind)
    check_c_source_compiles(
        "
        #include <dlfcn.h>
        int main()
        {
            int x = RTLD_DEEPBIND;
            return x;
        }
        "

        apitrace_has_rtld_deepbind

        FAIL_REGEX "warning: |error: "
        )

endmacro()

