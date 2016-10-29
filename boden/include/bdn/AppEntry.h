#ifndef BDN_AppEntry_H_
#define BDN_AppEntry_H_


#if BDN_PLATFORM_WIN32

	#include <bdn/win32/AppEntry.h>

	namespace bdn
	{
		typedef AppEntry bdn::win32::AppEntry;
	}


#elif BDN_PLATFORM_WINUWP

	#include <bdn/winuwp/AppEntry.h>

	namespace bdn
	{
		typedef AppEntry bdn::winuwp::AppEntry;
	}


#elif BDN_PLATFORM_LINUX

	#include <bdn/gtk/AppEntry.h>

	namespace bdn
	{
		typedef AppEntry bdn::gtk::AppEntry;
	}

#elif BDN_PLATFORM_MAC

	#include <bdn/mac/AppEntry.h>

	namespace bdn
	{
		typedef AppEntry bdn::mac::AppEntry;
	}


#elif BDN_PLATFORM_IOS

	#include <bdn/ios/AppEntry.h>

	namespace bdn
	{
		typedef AppEntry bdn::ios::AppEntry;
	}

#elif BDN_PLATFORM_ANDROID

	#include <bdn/android/AppEntry.h>

	namespace bdn
	{
		typedef AppEntry bdn::android::AppEntry;
	}


#elif BDN_PLATFORM_WEBEMS

	#include <bdn/webems/AppEntry.h>

	namespace bdn
	{
		typedef AppEntry bdn::webems::AppEntry;
	}

#else

#error Unknown platform - cannot determine AppEntry

#endif



#endif

