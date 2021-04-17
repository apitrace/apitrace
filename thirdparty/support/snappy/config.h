/* config.h.  Manually derived from config.h.in, with the subset of defines
 * that actually affect the library, given that most of them are meant for the
 * tests, which we don't build. */
#ifndef THIRD_PARTY_SNAPPY_OPENSOURCE_CMAKE_CONFIG_H_
#define THIRD_PARTY_SNAPPY_OPENSOURCE_CMAKE_CONFIG_H_

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

/* Define to 1 if you have a definition for mmap() in <sys/mman.h>. */
/* #undef HAVE_FUNC_MMAP */

/* Define to 1 if you have a definition for sysconf() in <unistd.h>. */
/* #undef HAVE_FUNC_SYSCONF */

/* Define to 1 to use the gflags package for command-line parsing. */
/* #undef HAVE_GFLAGS */

/* Define to 1 if you have Google Test. */
/* #undef HAVE_GTEST */

/* Define to 1 if you have the `lzo2' library (-llzo2). */
/* #undef HAVE_LIBLZO2 */

/* Define to 1 if you have the `z' library (-lz). */
/* #undef HAVE_LIBZ */

/* Define to 1 if you have the <sys/endian.h> header file. */
/* #undef HAVE_SYS_ENDIAN_H */

/* Define to 1 if you have the <sys/mman.h> header file. */
/* #undef HAVE_SYS_MMAN_H */

/* Define to 1 if you have the <sys/resource.h> header file. */
/* #undef HAVE_SYS_RESOURCE_H */

/* Define to 1 if you have the <sys/time.h> header file. */
/* #undef HAVE_SYS_TIME_H */

/* Define to 1 if you have the <sys/uio.h> header file. */
/* #undef HAVE_SYS_UIO_H */

/* Define to 1 if you have the <unistd.h> header file. */
#if !defined(_MSC_VER)
#  define HAVE_UNISTD_H 1
#endif

/* Define to 1 if you have the <windows.h> header file. */
#if defined(_WIN32)
#  define HAVE_WINDOWS_H 1
#endif

/* Define to 1 if you target processors with SSSE3+ and have <tmmintrin.h>. */
/* #undef SNAPPY_HAVE_SSSE3 */

/* Define to 1 if you target processors with BMI2+ and have <bmi2intrin.h>. */
/* #undef SNAPPY_HAVE_BMI2 */

/* Define to 1 if your processor stores words with the most significant byte
   first (like Motorola and SPARC, unlike Intel and VAX). */
#if defined(__GLIBC__)
#  include <endian.h>
#  if __BYTE_ORDER == __BIG_ENDIAN
#    define SNAPPY_IS_BIG_ENDIAN 1
#  endif
#elif defined(__APPLE__)
#  include <machine/endian.h>
#  if __DARWIN_BYTE_ORDER == __DARWIN_BIG_ENDIAN
#    define SNAPPY_IS_BIG_ENDIAN 1
#  endif
#endif

#endif  // THIRD_PARTY_SNAPPY_OPENSOURCE_CMAKE_CONFIG_H_
