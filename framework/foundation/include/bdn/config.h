#pragma once

#include <climits>
#include <cstddef>
#include <cstdint>
#include <ostream>

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

#ifdef BDN_PLATFORM_ANDROID
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

namespace bdn
{
    class config
    {
      public:
        constexpr static bool aggressive_float_optimization_enabled = BDN_AGGRESSIVE_FLOAT_OPTIMIZATIONS;
        constexpr static bool has_threads = BDN_HAVE_THREADS;
        constexpr static bool statics_are_thread_safe = BDN_COMPILER_STATIC_CONSTRUCTION_THREAD_SAFE;
        constexpr static bool is_big_endian = BDN_IS_BIG_ENDIAN;
        constexpr static bool is_little_endian = BDN_IS_LITTLE_ENDIAN;

        constexpr static int wchar_size = BDN_WCHAR_SIZE;

        constexpr static char target[] = BDN_TARGET;
        constexpr static char platform_family[] = BDN_PLATFORM_FAMILY;

        constexpr static bool is_ios =
#ifdef BDN_PLATFORM_IOS
            true
#else
            false
#endif
            ;

        constexpr static bool is_android =
#ifdef BDN_PLATFORM_ANDROID
            true
#else
            false
#endif
            ;
        constexpr static bool is_osx =
#ifdef BDN_PLATFORM_OSX
            true
#else
            false
#endif
            ;

        constexpr static bool is_family_posix =
#ifdef BDN_PLATFORM_FAMILY_POSIX
            true
#else
            false
#endif
            ;

        constexpr static bool is_family_windows =
#ifdef BDN_PLATFORM_FAMILY_WINDOWS
            true
#else
            false
#endif
            ;

        constexpr static bool uses_fk =
#ifdef BDN_USES_FK
            true
#else
            false
#endif
            ;

        constexpr static bool uses_java =
#ifdef BDN_USES_JAVA
            true
#else
            false
#endif
            ;
    };

    inline std::ostream &operator<<(std::ostream &os, config const & /*unused*/)
    {
        os << "target = " << config::target << std::endl;
        os << "platform_family = " << config::platform_family << std::endl;
        os << "aggressive_float_optimization_enabled = " << config::aggressive_float_optimization_enabled << std::endl;
        os << "has_threads = " << config::has_threads << std::endl;
        os << "statics_are_thread_safe = " << config::statics_are_thread_safe << std::endl;
        os << "is_big_endian = " << config::is_big_endian << std::endl;
        os << "is_little_endian = " << config::is_little_endian << std::endl;
        os << "wchar_size = " << config::wchar_size << std::endl;
        os << "is_ios = " << config::is_ios << std::endl;
        os << "is_android = " << config::is_android << std::endl;
        os << "is_osx = " << config::is_osx << std::endl;
        os << "is_family_posix = " << config::is_family_posix << std::endl;
        os << "is_family_windows = " << config::is_family_windows << std::endl;
        os << "uses_fk = " << config::uses_fk << std::endl;
        os << "uses_java = " << config::uses_java << std::endl;
        return os;
    }
}
