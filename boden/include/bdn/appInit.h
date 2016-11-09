#ifndef BDN_appInit_H_
#define BDN_appInit_H_

#include <bdn/AppControllerBase.h>
#include <bdn/IAppRunner.h>

#ifdef BDN_COMPILING_COMMANDLINE_APP
	#define BDN_APP_RUNNER_CLASS_NAME_ CommandLineAppRunner
#else
	#define BDN_APP_RUNNER_CLASS_NAME_ UiAppRunner
#endif


#if BDN_PLATFORM_WIN32

	#include <windows.h>

	#include <bdn/win32/UiAppRunner.h>
	#include <bdn/win32/CommandLineAppRunner.h>

	#ifdef BDN_COMPILING_COMMANDLINE_APP	

		#define BDN_APP_INIT_WITH_CONTROLLER_CREATOR( appControllerCreator )  \
			int main(int argc, char* argv[]) \
			{ \
                BDN_ROOT_BEGIN; \
				bdn::P< bdn::win32:: BDN_APP_RUNNER_CLASS_NAME_ > pAppRunner = bdn::newObj< bdn::win32:: BDN_APP_RUNNER_CLASS_NAME_ >( appControllerCreator, argc, argv ); \
                _setAppRunner(pAppRunner); \
				return pAppRunner->entry(); \
                BDN_ROOT_END_EXCEPTIONS_ARE_FATAL; \
			}

	#else

		#define BDN_APP_INIT_WITH_CONTROLLER_CREATOR( appControllerCreator )  \
			int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int showCommand) \
			{ \
                BDN_ROOT_BEGIN; \
				bdn::P< bdn::win32:: BDN_APP_RUNNER_CLASS_NAME_ > pAppRunner = bdn::newObj< bdn::win32:: BDN_APP_RUNNER_CLASS_NAME_ >( appControllerCreator, showCommand ); \
                _setAppRunner(pAppRunner); \
				return pAppRunner->entry(); \
                BDN_ROOT_END_EXCEPTIONS_ARE_FATAL; \
			}

	#endif

#elif BDN_PLATFORM_WINUWP

	#include <bdn/winuwp/AppRunner.h>

	#define BDN_APP_INIT_WITH_CONTROLLER_CREATOR( appControllerCreator )  \
		int __cdecl main(Platform::Array<Platform::String^>^ args) \
		{ \
            BDN_ROOT_BEGIN; \
			bdn::P< bdn::winuwp:: BDN_APP_RUNNER_CLASS_NAME_ > pAppRunner = bdn::newObj< bdn::winuwp:: BDN_APP_RUNNER_CLASS_NAME_ >( appControllerCreator, args ); \
            _setAppRunner(pAppRunner); \
			return pAppRunner->entry(); \
            BDN_ROOT_END_EXCEPTIONS_ARE_FATAL; \
		}

#elif BDN_PLATFORM_ANDROID

	#include <bdn/java/Env.h>

	#include <bdn/android/UiAppRunner.h>
	#include <bdn/android/CommandLineAppRunner.h>

	#define BDN_APP_INIT_WITH_CONTROLLER_CREATOR( appControllerCreator )  \
		extern "C" JNIEXPORT void JNICALL Java_io_boden_android_NativeInit_nativeLaunch( JNIEnv* pEnv, jclass cls ) \
		{ \
			BDN_JNI_BEGIN( pEnv ); \
			{ \
				bdn::P< bdn::android:: BDN_APP_RUNNER_CLASS_NAME_ > pAppRunner = bdn::newObj< bdn::android:: BDN_APP_RUNNER_CLASS_NAME_ >( appControllerCreator, args ); \
                _setAppRunner(pAppRunner); \
				pAppRunner->entry(); \
			} \
			BDN_JNI_END_EXCEPTIONS_ARE_FATAL; \
		}

#else

	#if BDN_PLATFORM_LINUX
		#include <bdn/gtk/CommandLineAppRunner.h>
		#include <bdn/gtk/UiAppRunner.h>
		#define BDN_APPRUNNER_ bdn::gtk:: BDN_APP_RUNNER_CLASS_NAME_

	#elif BDN_PLATFORM_MAC
		#include <bdn/mac/CommandLineAppRunner.h>
		#include <bdn/mac/UiAppRunner.h>
		#define BDN_APPRUNNER_ bdn::mac:: BDN_APP_RUNNER_CLASS_NAME_

	#elif BDN_PLATFORM_IOS
		#include <bdn/ios/CommandLineAppRunner.h>
		#include <bdn/ios/UiAppRunner.h>
		#define BDN_APPRUNNER_ bdn::ios:: BDN_APP_RUNNER_CLASS_NAME_

	#elif BDN_PLATFORM_WEBEMS
		#include <bdn/webems/CommandLineAppRunner.h>
		#include <bdn/webems/UiAppRunner.h>
		#define BDN_APPRUNNER_ bdn::webems:: BDN_APP_RUNNER_CLASS_NAME_

	#else

		#ifdef BDN_COMPILING_COMMANDLINE_APP
			// we can fall back to using the generic app runner for commandline apps
			#include <bdn/GenericCommandLineAppRunner.h>
			#define BDN_APPRUNNER_ bdn::GenericCommandLineAppRunner

		#else
			// there is no generic way to implement UI apps. So we have to bail out here
			#error Unsupported platform. Cannot create UI apps for unknown platforms.
		#endif

	#endif

    #define BDN_APP_INIT_WITH_CONTROLLER_CREATOR( appControllerCreator )  \
		int main(int argc, char* argv[]) \
		{ \
            BDN_ROOT_BEGIN; \
			bdn::P< BDN_APPRUNNER_ > pAppRunner = bdn::newObj< BDN_APPRUNNER_ >( appControllerCreator, argc, argv ); \
            _setAppRunner(pAppRunner); \
			return pAppRunner->entry(); \
            BDN_ROOT_END_EXCEPTIONS_ARE_FATAL; \
		}

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
#if BDN_COMPILING_COMMANDLINE_APP
	#include <bdn/CommandLineTestAppController.h>
	#define BDN_TEST_APP_INIT() BDN_APP_INIT( bdn::CommandLineTestAppController )
#else
	#include <bdn/UiTestAppController.h>
	#define BDN_TEST_APP_INIT() BDN_APP_INIT( bdn::UiTestAppController )
#endif


#endif


