#ifndef BDN_config_H_
#define BDN_config_H_

#include <cstdint>
#include <climits>
#include <cstddef>

#if WCHAR_MAX <= 0xffff
	#define BDN_WCHAR_SIZE 2

#elif WCHAR_MAX<=0xffffffff
	#define BDN_WCHAR_SIZE 4

#else
	#error Unsupported wchar size

#endif


// BDN_PLATFORM_WEBEMS cannot be detected automatically. It must be defined
// via compiler commandline options.
#if !defined(BDN_PLATFORM_DETECTED) && defined(BDN_PLATFORM_WEBEMS)
    #undef BDN_PLATFORM_WEBEMS
    #define BDN_PLATFORM_WEBEMS 1
    #define BDN_PLATFORM_DETECTED 1

#endif


#if !defined(BDN_PLATFORM_DETECTED) && defined(__cplusplus_cli)
	#define BDN_PLATFORM_DOTNET 1
	#define BDN_PLATFORM_DETECTED 1
#endif


#if !defined(BDN_PLATFORM_DETECTED) && (defined(WIN32) || defined(_WINDOWS) || defined(WINAPI_FAMILY) )

#if defined(WINAPI_FAMILY)

	#if WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP)
		#define BDN_PLATFORM_WIN32 1
	#else
		#define BDN_PLATFORM_WINUWP 1
	#endif

#else
	#define BDN_PLATFORM_WIN32 1

#endif

	#define BDN_PLATFORM_WINDOWS 1
	#define BDN_PLATFORM_DETECTED 1
	
#endif

#if !defined(BDN_PLATFORM_DETECTED) && defined(__APPLE__)

	#include <TargetConditionals.h>

	#if TARGET_OS_IPHONE
		#define BDN_PLATFORM_IOS 1
	#else
		#define BDN_PLATFORM_OSX 1
	#endif

	#define BDN_PLATFORM_DETECTED 1

#endif


#if !defined(BDN_PLATFORM_DETECTED) && (defined(__ANDROID__) || defined(ANDROID))
	#define BDN_PLATFORM_ANDROID 1
	#define BDN_PLATFORM_DETECTED 1
#endif


#if !defined(BDN_PLATFORM_DETECTED) && defined(__linux__)
	#define BDN_PLATFORM_LINUX 1
	#define BDN_PLATFORM_DETECTED 1

#endif

#if !defined(BDN_PLATFORM_DETECTED) && defined(_POSIX_VERSION)
	// some generic Posix/Unix system
	#define BDN_PLATFORM_POSIX 1
	#define BDN_PLATFORM_DETECTED 1
#endif


#if !defined(BDN_PLATFORM_DETECTED)
	#error Unable to determine target system type

#endif



#if !defined(BDN_PLATFORM_POSIX)
	#if BDN_PLATFORM_IOS || BDN_PLATFORM_OSX || BDN_PLATFORM_LINUX || BDN_PLATFORM_ANDROID
		#define BDN_PLATFORM_POSIX 1
    #endif
#endif


#if BDN_PLATFORM_WEBEMS

	#if __EMSCRIPTEN_major__<1 || (__EMSCRIPTEN_major__==1 && __EMSCRIPTEN_minor__ < 36 )
		// Older emscripten versions had compiler bugs that cause major troubles with some integral
		// parts of the library (exception handling not working correctly in some cases, etc.).
		// So we cannot use those.
		#pragma error("Emscripten 1.36.0 or higher required.")
	#endif


    // no support for threads (yet). There is actually an implementation from Emscripten
    // but as of the time of this writing it only works if the Javascript code is executed
    // with Firefox nightly. It depends on an extension of the web worker specification, which
    // is not standardized yet. So we disable threading support.
    #define BDN_HAVE_THREADS 0

#else
    // all other platforms support multi-threading
    #define BDN_HAVE_THREADS 1

#endif


// The C++11 standard requires that local static variables
// must be initialized in a thread-safe manner (so that they are
// constructed exactly once). This feature is also called "magic statics"
// by some.

#if defined(_MSC_VER) && _MSC_VER<1700
// Visual Studio versions before 2015 did not support the thread
// safe static requirement.
#define BDN_COMPILER_STATIC_CONSTRUCTION_THREAD_SAFE 0

#else
// since the standard demands this, we assume that statics are safe by default.
#define BDN_COMPILER_STATIC_CONSTRUCTION_THREAD_SAFE 1
#endif


#if BDN_PLATFORM_ANDROID
// the c++_shared and c++_static standard libraries do not implement the full locale
// api and the android docs state that support for wchar_t is "limited".
// So we hard-code UTF-8 encoding - it is the only encoding that android supports anyway.
#define BDN_OVERRIDE_LOCALE_ENCODING_UTF8 1
#endif


#ifndef BDN_IS_BIG_ENDIAN

	#if defined(BDN_IS_LITTLE_ENDIAN)

		#if BDN_IS_LITTLE_ENDIAN
			#define BDN_IS_BIG_ENDIAN 0
		#else
			#define BDN_IS_BIG_ENDIAN 1
		#endif

	// it seems that on macs one of the macros __LITTLE_ENDIAN__ or __BIG_ENDIAN__ is always defined by the compiler
	// (architecture/byte_order.h uses these).
	// So the code below should definitely cover macOS

	#elif defined(__BIG_ENDIAN__) \
		|| defined(__ARMEB__) \
		|| defined(__AARCH64EB__) \
		|| defined(__THUMBEB__) \
		|| defined(_MIPSEB) \
		|| defined(__MIPSEB) \
		|| defined(__MIPSEB__) \
		|| ( defined(__BYTE_ORDER) && defined(__BIG_ENDIAN) && __BYTE_ORDER==__BIG_ENDIAN ) \
		|| ( defined(__BYTE_ORDER__) && defined(__ORDER_BIG_ENDIAN__) && __BYTE_ORDER__==__ORDER_BIG_ENDIAN__ )  \
		|| ( defined(BYTE_ORDER) && defined(BIG_ENDIAN) && BYTE_ORDER==BIG_ENDIAN ) 

			#define	BDN_IS_BIG_ENDIAN 1

	#elif defined(__LITTLE_ENDIAN__) \
		|| defined(__ARMEL__) \
		|| defined(__AARCH64EL__) \
		|| defined(__THUMBEL__) \
		|| defined(_MIPSEL) \
		|| defined(__MIPSEL) \
		|| defined(__MIPSEL__) \
		|| ( defined(__BYTE_ORDER) && defined(__LITTLE_ENDIAN) && __BYTE_ORDER==__LITTLE_ENDIAN ) \
		|| ( defined(__BYTE_ORDER__) && defined(__ORDER_LITTLE_ENDIAN__) && __BYTE_ORDER__==__ORDER_LITTLE_ENDIAN__ )  \
		|| ( defined(BYTE_ORDER) && defined(LITTLE_ENDIAN) && BYTE_ORDER==LITTLE_ENDIAN ) 

			#define	BDN_IS_BIG_ENDIAN 0


	#elif BDN_PLATFORM_WINDOWS
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

		#if defined(__BIG_ENDIAN__) \
			|| defined(__ARMEB__) \
			|| defined(__AARCH64EB__) \
			|| defined(__THUMBEB__) \
			|| defined(_MIPSEB) \
			|| defined(__MIPSEB) \
			|| defined(__MIPSEB__) \
			|| ( defined(__BYTE_ORDER) && defined(__BIG_ENDIAN) && __BYTE_ORDER==__BIG_ENDIAN ) \
			|| ( defined(__BYTE_ORDER__) && defined(__ORDER_BIG_ENDIAN__) && __BYTE_ORDER__==__ORDER_BIG_ENDIAN__ )  \
			|| ( defined(BYTE_ORDER) && defined(BIG_ENDIAN) && BYTE_ORDER==BIG_ENDIAN ) 

				#define	BDN_IS_BIG_ENDIAN 1

		#elif defined(__LITTLE_ENDIAN__) \
			|| defined(__ARMEL__) \
			|| defined(__AARCH64EL__) \
			|| defined(__THUMBEL__) \
			|| defined(_MIPSEL) \
			|| defined(__MIPSEL) \
			|| defined(__MIPSEL__) \
			|| ( defined(__BYTE_ORDER) && defined(__LITTLE_ENDIAN) && __BYTE_ORDER==__LITTLE_ENDIAN ) \
			|| ( defined(__BYTE_ORDER__) && defined(__ORDER_LITTLE_ENDIAN__) && __BYTE_ORDER__==__ORDER_LITTLE_ENDIAN__ )  \
			|| ( defined(BYTE_ORDER) && defined(LITTLE_ENDIAN) && BYTE_ORDER==LITTLE_ENDIAN ) 

				#define	BDN_IS_BIG_ENDIAN 0

		#else

			#error "unable to determine endian-ness of CPU architecture. Define BDN_IS_BIG_ENDIAN as a preprocessor macro and set it to either 1 or 0 (depending on the target CPU architecture) to solve this."

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

// Compiler Issue Workarounds

// Visual Studio 2015 to 2017 (v15.5)
#if defined(_MSC_VER) && _MSC_VER>=1900 && _MSC_VER<=1912
	// Missing specializations for std::use_facet for char16_t and char32_t
	#define BDN_STD_USE_FACET_MISSING_SPECIALIZATION 1
	// Bug when using char16_t and char32_t with std::codecvt
	#define BDN_STD_CODECVT_CHAR16_CHAR32_BUG 1
#else
	#define BDN_STD_USE_FACET_MISSING_SPECIALIZATION 0
	#define BDN_STD_CODECVT_CHAR16_CHAR32_BUG 0
#endif

// Visual Studio up to 2017 (v15.5)
#if defined(_MSC_VER) && _MSC_VER<=1912
	// Compiler bug with VS2015/VS2017 in SequenceFilter
	#define BDN_SEQUENCE_FILTER_COMPILER_BUG 1
	// std::isnan missing
	#define BDN_STD_ISNAN_INT_MISSING 1
	// std::isfinite missing for type int
	#define BDN_STD_ISFINITE_INT_MISSING 1
#else
	#define BDN_SEQUENCE_FILTER_COMPILER_BUG 0
	#define BDN_STD_ISNAN_INT_MISSING 0
	#define BDN_STD_ISFINITE_INT_MISSING 0
#endif

#endif


