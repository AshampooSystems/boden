#ifndef BDN_config_H_
#define BDN_config_H_

#include <limits.h>

#if WCHAR_MAX<=0xffff
	#define BDN_WCHAR_SIZE 2

#elif WCHAR_MAX<=0xffffffff
	#define BDN_WCHAR_SIZE 4

#else
	#error Unsupported wchar size

#endif

#if !defined(BDN_TARGET_DETECTED) && (defined(WIN32) || defined(_WINDOWS))
	#define BDN_TARGET_WINDOWS 1
	#define BDN_TARGET_DETECTED 1

#else
	#define BDN_TARGET_WINDOWS 0

#endif

#if !defined(BDN_TARGET_DETECTED) && defined(__APPLE__)

	#include <TargetConditionals.h>

	#if TARGET_OS_IPHONE
		#define BDN_TARGET_IOS 1
		#define BDN_TARGET_OSX 0	
	#else
		#define BDN_TARGET_IOS 0
		#define BDN_TARGET_OSX 1
	#endif

	#define BDN_TARGET_DETECTED 1

#else
	#define BDN_TARGET_IOS 0
	#define BDN_TARGET_OSX 0

#endif


#if !defined(BDN_TARGET_DETECTED) && (defined(__ANDROID__) || defined(ANDROID))
	#define BDN_TARGET_ANDROID 1
	#define BDN_TARGET_DETECTED 1

#else
	#define BDN_TARGET_ANDROID 0

#endif


#if !defined(BDN_TARGET_DETECTED) && defined(__linux__)
	#define BDN_TARGET_LINUX 1
	#define BDN_TARGET_DETECTED 1

#else
	#define BDN_TARGET_LINUX 0

#endif

#if !defined(BDN_TARGET_DETECTED) && defined(_POSIX_VERSION)
	// some generic Posix/Unix system
	#define BDN_TARGET_POSIX 1
	#define BDN_TARGET_DETECTED 1

#endif


#if !defined(BDN_TARGET_DETECTED)
	#error Unable to determine target system type

#endif


#if !defined(BDN_TARGET_POSIX)
	#if BDN_TARGET_IOS || BDN_TARGET_OSX || BDN_TARGET_LINUX
		#define BDN_TARGET_POSIX 1
	#else
		#define BDN_TARGET_POSIX 0
	#endif

#endif



#endif


