/* config.h.  Manually derived from config.h.in, with the subset of defines
 * that actually affect the library, given that most of them are meant for the
 * tests, which we don't build. */

/* Define to 1 if the compiler supports __builtin_ctz and friends. */
#if defined(__GNUCC__)
#  define HAVE_BUILTIN_CTZ 1
#endif

/* Define to 1 if the compiler supports __builtin_expect. */
#if defined(__GNUCC__)
# define HAVE_BUILTIN_EXPECT 1
#endif

/* Define to 1 if you have the <byteswap.h> header file. */
#if defined(__linux__)
#  define HAVE_BYTESWAP_H 1
#endif

/* Define to 1 if you have the <dlfcn.h> header file. */
#undef HAVE_DLFCN_H

/* Use the gflags package for command-line parsing. */
#undef HAVE_GFLAGS

/* Defined when Google Test is available. */
#undef HAVE_GTEST

/* Define to 1 if you have the <inttypes.h> header file. */
#define HAVE_INTTYPES_H 1

/* Define to 1 if you have the `fastlz' library (-lfastlz). */
#undef HAVE_LIBFASTLZ

/* Define to 1 if you have the `lzf' library (-llzf). */
#undef HAVE_LIBLZF

/* Define to 1 if you have the `lzo2' library (-llzo2). */
#undef HAVE_LIBLZO2

/* Define to 1 if you have the `quicklz' library (-lquicklz). */
#undef HAVE_LIBQUICKLZ

/* Define to 1 if you have the `z' library (-lz). */
#undef HAVE_LIBZ

/* Define to 1 if you have the <memory.h> header file. */
#undef HAVE_MEMORY_H

/* Define to 1 if you have the <stddef.h> header file. */
#define HAVE_STDDEF_H 1

/* Define to 1 if you have the <stdint.h> header file. */
#define HAVE_STDINT_H 1

/* Define to 1 if you have the <stdlib.h> header file. */
#define HAVE_STDLIB_H 1

/* Define to 1 if you have the <strings.h> header file. */
#undef HAVE_STRINGS_H

/* Define to 1 if you have the <string.h> header file. */
#define HAVE_STRING_H 1

/* Define to 1 if you have the <sys/byteswap.h> header file. */
#undef HAVE_SYS_BYTESWAP_H

/* Define to 1 if you have the <sys/endian.h> header file. */
#undef HAVE_SYS_ENDIAN_H

/* Define to 1 if you have the <sys/mman.h> header file. */
#undef HAVE_SYS_MMAN_H

/* Define to 1 if you have the <sys/resource.h> header file. */
#undef HAVE_SYS_RESOURCE_H

/* Define to 1 if you have the <sys/stat.h> header file. */
#undef HAVE_SYS_STAT_H

/* Define to 1 if you have the <sys/types.h> header file. */
#define HAVE_SYS_TYPES_H 1

/* Define to 1 if you have the <unistd.h> header file. */
#if !defined(_WIN32)
#  define HAVE_UNISTD_H 1
#endif

/* Define to 1 if you have the <windows.h> header file. */
#if defined(_WIN32)
#  define HAVE_WINDOWS_H 1
#endif

/* Define to the sub-directory in which libtool stores uninstalled libraries.
   */
#undef LT_OBJDIR

/* Name of package */
#undef PACKAGE

/* Define to the address where bug reports for this package should be sent. */
#undef PACKAGE_BUGREPORT

/* Define to the full name of this package. */
#undef PACKAGE_NAME

/* Define to the full name and version of this package. */
#undef PACKAGE_STRING

/* Define to the one symbol short name of this package. */
#undef PACKAGE_TARNAME

/* Define to the home page for this package. */
#undef PACKAGE_URL

/* Define to the version of this package. */
#undef PACKAGE_VERSION

/* Define to 1 if you have the ANSI C header files. */
#define STDC_HEADERS 1

/* Version number of package */
#undef VERSION

/* Define WORDS_BIGENDIAN to 1 if your processor stores words with the most
   significant byte first (like Motorola and SPARC, unlike Intel). */
#if defined(__GLIBC__)
#  include <endian.h>
#  if __BYTE_ORDER == __BIG_ENDIAN
#    define WORDS_BIGENDIAN 1
#  endif
#elif defined(__APPLE__)
#  include <machine/endian.h>
#  if __DARWIN_BYTE_ORDER == __DARWIN_BIG_ENDIAN
#    define WORDS_BIGENDIAN 1
#  endif
#endif
