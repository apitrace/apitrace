#    CMakeLists.txt -- libbacktrace CMake build script
#    Contributed by Alexander Monakov, ISP RAS
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are
# met:
#
#     (1) Redistributions of source code must retain the above copyright
#     notice, this list of conditions and the following disclaimer.
#
#     (2) Redistributions in binary form must reproduce the above copyright
#     notice, this list of conditions and the following disclaimer in
#     the documentation and/or other materials provided with the
#     distribution.
#
#     (3) The name of the author may not be used to
#     endorse or promote products derived from this software without
#     specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
# IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT,
# INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
# SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
# HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
# STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
# IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.  */

cmake_minimum_required (VERSION 3.5)

project (libbacktrace)

set (CMAKE_INTERPROCEDURAL_OPTIMIZATION FALSE)
set (BACKTRACE_SUPPORTED 1)

include (CheckSymbolExists)
check_symbol_exists (_Unwind_Backtrace unwind.h HAVE_BACKTRACE)
check_symbol_exists (_Unwind_GetIPInfo unwind.h HAVE_GETIPINFO)

if (HAVE_BACKTRACE)
    set (BACKTRACE_FILE libbacktrace/backtrace.c libbacktrace/simple.c)
else ()
    set (BACKTRACE_FILE libbacktrace/nounwind.c)
    set (BACKTRACE_SUPPORTED 0)
    message (STATUS "libunwind not found. Disabling Backtrace support.")
endif ()

include (CheckCCompilerFlag)
check_c_compiler_flag ("-funwind-tables" FLAG_UNWIND_TABLES)
if (FLAG_UNWIND_TABLES)
    add_definitions ("-funwind-tables")
endif ()

# Adjust warnings
if (NOT MSVC)
    add_definitions ("-Wno-switch -Wno-enum-compare")
endif ()

check_c_source_compiles (
    "int i;
    int main() {
    __sync_bool_compare_and_swap (&i, i, i);
    __sync_lock_test_and_set (&i, 1);
    __sync_lock_release (&i);}"
    HAVE_SYNC_FUNCTIONS)

check_c_source_compiles (
    "int i;
    int main() {
    __atomic_load_n (&i, __ATOMIC_ACQUIRE);
    __atomic_store_n (&i, i, __ATOMIC_RELEASE);}"
    HAVE_ATOMIC_FUNCTIONS)

if (HAVE_SYNC_FUNCTIONS)
    set (BACKTRACE_SUPPORTS_THREADS 1)
else ()
    set (BACKTRACE_SUPPORTS_THREADS 0)
endif ()

if (CMAKE_EXECUTABLE_FORMAT STREQUAL "ELF")
    set (FORMAT_FILE libbacktrace/elf.c libbacktrace/dwarf.c)
    math (EXPR BACKTRACE_ELF_SIZE 8*${CMAKE_C_SIZEOF_DATA_PTR})
    set (BACKTRACE_SUPPORTS_DATA 1)
else ()
    set (FORMAT_FILE libbacktrace/unknown.c)
    set (BACKTRACE_SUPPORTED 0)
    message (STATUS "Executable format is not ELF. Disabling Backtrace support.")
    set (BACKTRACE_SUPPORTS_DATA 0)
endif ()

check_symbol_exists (mmap sys/mman.h HAVE_MMAP)

if (HAVE_MMAP)
    set (VIEW_FILE libbacktrace/mmapio.c)
    check_symbol_exists (MAP_ANONYMOUS sys/mman.h HAVE_MMAP_ANONYMOUS)
    check_symbol_exists (MAP_ANON sys/mman.h HAVE_MMAP_ANON)
    if (HAVE_MMAP_ANONYMOUS AND HAVE_MMAP_ANON)
        set (ALLOC_FILE libbacktrace/mmap.c)
    else ()
        set (ALLOC_FILE libbacktrace/alloc.c)
    endif ()
else ()
    set (VIEW_FILE libbacktrace/read.c)
    set (ALLOC_FILE libbacktrace/alloc.c)
endif ()

if (ALLOC_FILE STREQUAL "libbacktrace/alloc.c")
    set (BACKTRACE_USES_MALLOC 1)
else ()
    set (BACKTRACE_USES_MALLOC 0)
endif ()

add_definitions (-D_GNU_SOURCE)
set (CMAKE_REQUIRED_DEFINITIONS ${CMAKE_REQUIRED_DEFINITIONS} -D_GNU_SOURCE)
check_symbol_exists (dl_iterate_phdr link.h HAVE_DL_ITERATE_PHDR)

include (CheckFunctionExists)
check_function_exists (fcntl HAVE_FCNTL)

check_function_exists (strnlen HAVE_DECL_STRNLEN)

check_function_exists (getexecname HAVE_GETEXECNAME)

include (CheckIncludeFile)
check_include_file ("link.h" HAVE_LINK_H)
check_include_file ("stdint.h" HAVE_STDINT_H)
check_include_file ("sys/ldr.h" HAVE_SYS_LDR_H)
check_include_file ("sys/link.h" HAVE_SYS_LINK_H)
check_include_file ("sys/mman.h" HAVE_SYS_MMAN_H)
check_include_file ("sys/types.h" HAVE_SYS_TYPES_H)
set (HAVE_ZLIB 1)

configure_file (libbacktrace/backtrace-supported.h.in support/libbacktrace/backtrace-supported.h)

configure_file (support/libbacktrace/config.h.in support/libbacktrace/config.h)

add_convenience_library (backtrace EXCLUDE_FROM_ALL
    ${BACKTRACE_FILE}
    ${FORMAT_FILE}
    ${VIEW_FILE}
    ${ALLOC_FILE}
    libbacktrace/atomic.c
    libbacktrace/fileline.c
    libbacktrace/posix.c
    libbacktrace/print.c
    libbacktrace/state.c
    libbacktrace/sort.c
    libbacktrace/testlib.c
)
target_include_directories (backtrace
    PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/support/libbacktrace
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/libbacktrace
)
target_link_libraries (backtrace PUBLIC ZLIB::ZLIB ${CMAKE_DL_LIBS})

add_executable (btest libbacktrace/btest.c)
set_target_properties (btest PROPERTIES
    COMPILE_FLAGS "${CMAKE_C_FLAGS_DEBUG}"
    )
target_include_directories (btest PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/support/libbacktrace)
target_link_libraries (btest backtrace)
add_dependencies (check btest)
add_test (
    NAME libbacktrace_btest
    COMMAND
        # Workaround https://gitlab.kitware.com/cmake/cmake/-/issues/18863
        ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/support/libbacktrace/closefds.py
        $<TARGET_FILE:btest>
)

add_executable (stest libbacktrace/stest.c)
set_target_properties (stest PROPERTIES
    COMPILE_FLAGS "${CMAKE_C_FLAGS_DEBUG}"
    )
target_include_directories (stest PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/support/libbacktrace)
target_link_libraries (stest backtrace)
add_dependencies (check stest)
add_test (NAME libbacktrace_stest COMMAND stest)

install (
    FILES libbacktrace/LICENSE
    DESTINATION ${DOC_INSTALL_DIR}
    RENAME LICENSE-libbacktrace.txt
)
