/**
 * This file has no copyright assigned and is placed in the Public Domain.
 * This file is part of the mingw-w64 runtime package.
 * No warranty is given; refer to the file DISCLAIMER.PD within this package.
 */

#ifndef SAL_HXX
#define SAL_HXX

#ifdef __GNUC__
#  define __inner_checkReturn __attribute__((warn_unused_result))
#elif defined(_MSC_VER)
#  define __inner_checkReturn __declspec("SAL_checkReturn")
#else
#  define __inner_checkReturn
#endif

#define __checkReturn __inner_checkReturn

#define _In_
#define _In_opt_
#define _Out_

#define _Struct_size_bytes_(size)

#endif

