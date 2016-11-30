#ifndef BDN_appInit_H_
#define BDN_appInit_H_

#include <bdn/AppControllerBase.h>

#if BDN_PLATFORM_WIN32

    #include <bdn/win32/appEntry.h>

	#ifdef BDN_COMPILING_COMMANDLINE_APP	
		#define BDN_APP_INIT_WITH_CONTROLLER_CREATOR( appControllerCreator )  \
			int main(int argc, char* argv[]) \
			{ \
                return bdn::win32::commandLineAppEntry(appControllerCreator, argc, argv); \
			}

	#else
		#define BDN_APP_INIT_WITH_CONTROLLER_CREATOR( appControllerCreator )  \
			int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int showCommand) \
			{ \
                return bdn::win32::uiAppEntry(appControllerCreator, showCommand; \
			}

	#endif

#elif BDN_PLATFORM_WINUWP

	#include <bdn/winuwp/appEntry.h>

	#define BDN_APP_INIT_WITH_CONTROLLER_CREATOR( appControllerCreator )  \
		int __cdecl main(Platform::Array<Platform::String^>^ args) \
		{ \
            return bdn::winuwp::appEntry(appControllerCreator, args); \
		}

#elif BDN_PLATFORM_ANDROID

	#include <bdn/android/appEntry.h>

	#define BDN_APP_INIT_WITH_CONTROLLER_CREATOR( appControllerCreator )  \
		extern "C" JNIEXPORT void JNICALL Java_io_boden_android_NativeInit_nativeLaunch( JNIEnv* pEnv, jclass cls ) \
		{ \
            bdn::android::appEntry(appControllerCreator, pEnv); \
		}


#elif BDN_PLATFORM_LINUX

    #ifdef BDN_COMPILING_COMMANDLINE_APP
        #include <bdn/genericAppEntry.h>

        #define BDN_APP_INIT_WITH_CONTROLLER_CREATOR( appControllerCreator )  \
            int main(int argc, char* argv[]) \
            { \
                return bdn::genericCommandLineAppEntry(appControllerCreator, argc, argv); \
            }

    #else
        #include <bdn/gtk/appEntry.h>

        #define BDN_APP_INIT_WITH_CONTROLLER_CREATOR( appControllerCreator )  \
            int main(int argc, char* argv[]) \
            { \
                return bdn::gtk::uiAppEntry(appControllerCreator, argc, argv); \
            }

    #endif


#elif BDN_PLATFORM_OSX

    #ifdef BDN_COMPILING_COMMANDLINE_APP
        #include <bdn/genericAppEntry.h>

        #define BDN_APP_INIT_WITH_CONTROLLER_CREATOR( appControllerCreator )  \
            int main(int argc, char* argv[]) \
            { \
                return bdn::genericCommandLineAppEntry(appControllerCreator, argc, argv); \
            }

    #else
        #include <bdn/mac/appEntry.h>

        #define BDN_APP_INIT_WITH_CONTROLLER_CREATOR( appControllerCreator )  \
            int main(int argc, char* argv[]) \
            { \
                return bdn::mac::uiAppEntry(appControllerCreator, argc, argv); \
            }

    #endif

#elif BDN_PLATFORM_IOS

    #include <bdn/ios/appEntry.h>

    #define BDN_APP_INIT_WITH_CONTROLLER_CREATOR( appControllerCreator )  \
        int main(int argc, char* argv[]) \
        { \
            return bdn::ios::appEntry(appControllerCreator, argc, argv); \
        }

#elif BDN_PLATFORM_WEBEMS

    #include <bdn/webems/appEntry.h>

    #define BDN_APP_INIT_WITH_CONTROLLER_CREATOR( appControllerCreator )  \
        int main(int argc, char* argv[]) \
        { \
            return bdn::webems::appEntry(appControllerCreator, argc, argv); \
        }

#else

    #ifdef BDN_COMPILING_COMMANDLINE_APP

        #include <bdn/appEntry.h>

        #define BDN_APP_INIT_WITH_CONTROLLER_CREATOR( appControllerCreator )  \
            int main(int argc, char* argv[]) \
            { \
                return bdn::genericCommandLineAppEntry(appControllerCreator, argc, argv); \
            }

    #else

        static_assert(false, "Unknown target platform - don't know how to define app entry point.");

    #endif

#endif



/** \def BDN_APP_INIT( appControllerClass )
 
    Sets up the app. This macro must be used in every Boden app.
	Among other things, it sets up the main entry point for the
	app, ensures that the Boden framework is properly initialized
	and sets up the app controller.
 
    appControllerClass is the class of the app's controller object. The app
    controller controls the startup and shutdown of your app and should
    be derived from AppControllerBase.

	Note that for commandline apps or high performance apps the app controller can also
	implement a "main loop" that performs the actual work of the app.
 
    This is how BDN_APP_INIT is used:
 
    \code

	#include <bdn/init.h>
	#include <bdn/appInit.h>
 
    class MyAppController : public bdn::AppControllerBase
    {
    public:
        ... see AppControllerBase for functions to implement here
    };
 
    BDN_APP_INIT( MyAppController );
 
    \endcode
 
 */
#define BDN_APP_INIT(appControllerClass) BDN_APP_INIT_WITH_CONTROLLER_CREATOR( (( [](){ return bdn::newObj<appControllerClass>(); } )) )



/** \def BDN_TEST_APP_INIT()

	Sets up a Boden test framework unittest app that runs the test cases defined in the app.

	This replaces the normal BDN_APP_INIT() statement that you use for normal apps.

	*/
#if BDN_COMPILING_COMMANDLINE_APP && (BDN_PLATFORM_WIN32 || BDN_PLATFORM_LINUX || BDN_PLATFORM_MAC)
	#include <bdn/CommandLineTestAppController.h>
	#define BDN_TEST_APP_INIT() BDN_APP_INIT( bdn::CommandLineTestAppController )
#else
	#include <bdn/UiTestAppController.h>
	#define BDN_TEST_APP_INIT() BDN_APP_INIT( bdn::UiTestAppController )
#endif


#endif


