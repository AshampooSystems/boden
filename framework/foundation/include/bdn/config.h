#ifndef BDN_config_H_
#define BDN_config_H_

#include <cstdint>
#include <climits>
#include <cstddef>

#include <bdn/platform.h>

#if WCHAR_MAX <= 0xffff
#define BDN_WCHAR_SIZE 2
#elif WCHAR_MAX <= 0xffffffff
#define BDN_WCHAR_SIZE 4
#else
#error Unsupported wchar size
#endif

#define BDN_HAVE_THREADS 1
#define BDN_COMPILER_STATIC_CONSTRUCTION_THREAD_SAFE 1

#if BDN_PLATFORM_ANDROID
// the c++_shared and c++_static standard libraries do not implement the full
// locale api and the android docs state that support for wchar_t is "limited".
// So we hard-code UTF-8 encoding - it is the only encoding that android
// supports anyway.
#define BDN_OVERRIDE_LOCALE_ENCODING_UTF8 1
#endif

#include "config.endian.h"

/** \def BDN_AGGRESSIVE_FLOAT_OPTIMIZATIONS

    This macro is 1 if the compiler performs aggressive floating point
   optimizations.

    In particular, the following should be assumed when this is set:

    - The result of comparisons or arithmetic operations involving NaN or
   infinity values should be regarded as undefined.
    - +0 and -0 are interchangeable
    - Operations involving finite numbers work mostly as expected, but may
   sometimes have reduced precision.
 */
#ifndef BDN_AGGRESSIVE_FLOAT_OPTIMIZATIONS
#ifdef __FAST_MATH__
#define BDN_AGGRESSIVE_FLOAT_OPTIMIZATIONS 1
#else
#define BDN_AGGRESSIVE_FLOAT_OPTIMIZATIONS 0
#endif
#endif

#endif
