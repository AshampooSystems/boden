#pragma once

#ifndef BDN_IS_BIG_ENDIAN
#if defined(BDN_IS_LITTLE_ENDIAN)

#if BDN_IS_LITTLE_ENDIAN
#define BDN_IS_BIG_ENDIAN 0
#else
#define BDN_IS_BIG_ENDIAN 1
#endif

// it seems that on macs one of the macros __LITTLE_ENDIAN__ or __BIG_ENDIAN__
// is always defined by the compiler (architecture/byte_order.h uses these). So
// the code below should definitely cover macOS

#elif defined(__BIG_ENDIAN__) || defined(__ARMEB__) || defined(__AARCH64EB__) || defined(__THUMBEB__) ||               \
    defined(_MIPSEB) || defined(__MIPSEB) || defined(__MIPSEB__) ||                                                    \
    (defined(__BYTE_ORDER) && defined(__BIG_ENDIAN) && __BYTE_ORDER == __BIG_ENDIAN) ||                                \
    (defined(__BYTE_ORDER__) && defined(__ORDER_BIG_ENDIAN__) && __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__) ||            \
    (defined(BYTE_ORDER) && defined(BIG_ENDIAN) && BYTE_ORDER == BIG_ENDIAN)

#define BDN_IS_BIG_ENDIAN 1

#elif defined(__LITTLE_ENDIAN__) || defined(__ARMEL__) || defined(__AARCH64EL__) || defined(__THUMBEL__) ||            \
    defined(_MIPSEL) || defined(__MIPSEL) || defined(__MIPSEL__) ||                                                    \
    (defined(__BYTE_ORDER) && defined(__LITTLE_ENDIAN) && __BYTE_ORDER == __LITTLE_ENDIAN) ||                          \
    (defined(__BYTE_ORDER__) && defined(__ORDER_LITTLE_ENDIAN__) && __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__) ||      \
    (defined(BYTE_ORDER) && defined(LITTLE_ENDIAN) && BYTE_ORDER == LITTLE_ENDIAN)

#define BDN_IS_BIG_ENDIAN 0

#elif BDN_PLATFORM_FAMILY_WINDOWS
// Windows is always little endian. Even on ARM machines.
#define BDN_IS_BIG_ENDIAN 0

#elif BDN_PLATFORM_IOS
// iOS is little endian
#define BDN_IS_BIG_ENDIAN 0

#else

// Last chance: try to include glibc endian.h
// If this causes an error on your system then you need to
// defined BDN_IS_BIG_ENDIAN as a preprocessor macro
// with a value of either 1 or 0, depending on the endianness
// of your system
#include <endian.h>

// now check all the macros from above again

#if defined(__BIG_ENDIAN__) || defined(__ARMEB__) || defined(__AARCH64EB__) || defined(__THUMBEB__) ||                 \
    defined(_MIPSEB) || defined(__MIPSEB) || defined(__MIPSEB__) ||                                                    \
    (defined(__BYTE_ORDER) && defined(__BIG_ENDIAN) && __BYTE_ORDER == __BIG_ENDIAN) ||                                \
    (defined(__BYTE_ORDER__) && defined(__ORDER_BIG_ENDIAN__) && __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__) ||            \
    (defined(BYTE_ORDER) && defined(BIG_ENDIAN) && BYTE_ORDER == BIG_ENDIAN)

#define BDN_IS_BIG_ENDIAN 1

#elif defined(__LITTLE_ENDIAN__) || defined(__ARMEL__) || defined(__AARCH64EL__) || defined(__THUMBEL__) ||            \
    defined(_MIPSEL) || defined(__MIPSEL) || defined(__MIPSEL__) ||                                                    \
    (defined(__BYTE_ORDER) && defined(__LITTLE_ENDIAN) && __BYTE_ORDER == __LITTLE_ENDIAN) ||                          \
    (defined(__BYTE_ORDER__) && defined(__ORDER_LITTLE_ENDIAN__) && __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__) ||      \
    (defined(BYTE_ORDER) && defined(LITTLE_ENDIAN) && BYTE_ORDER == LITTLE_ENDIAN)

#define BDN_IS_BIG_ENDIAN 0

#else

#error                                                                                                                 \
    "unable to determine endian-ness of CPU architecture. Define BDN_IS_BIG_ENDIAN as a preprocessor macro and set it to either 1 or 0 (depending on the target CPU architecture) to solve this."

#endif

#endif

#endif

#ifdef BDN_IS_LITTLE_ENDIAN
#undef BDN_IS_LITTLE_ENDIAN
#endif

#if BDN_IS_BIG_ENDIAN
#define BDN_IS_LITTLE_ENDIAN 0
#else
#define BDN_IS_LITTLE_ENDIAN 1
#endif