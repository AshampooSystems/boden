#ifndef BDN_appInit_H_
#define BDN_appInit_H_

#include <bdn/AppControllerBase.h>
#include <bdn/CommandLineAppController.h>
#include <bdn/TestAppWithUiController.h>

#include <map>
#include <vector>

namespace bdn
{
    

/** Performs common global initialization that should be done at the start of every program.*/
void _mainInit();

#if BDN_PLATFORM_WINRT

	int _commandLineAppMain(	std::function< int(const AppLaunchInfo& launchInfo) > appFunc,
								AppControllerBase* pAppController,
								Platform::Array<Platform::String^>^ args );

#else
	
	int _commandLineAppMain(	std::function< int(const AppLaunchInfo& launchInfo) > appFunc,
								AppControllerBase* pAppController,
								int argCount,
								char* argv[] );

#endif


template<class ControllerType = CommandLineAppController>
inline P<ControllerType> _createCommandLineAppController()
{
	return newObj<ControllerType>();	
}
    

int _commandLineTestAppFunc( const AppLaunchInfo& launchInfo );


#if BDN_PLATFORM_WIN32
	int _uiAppMain(AppControllerBase* pAppController, int showCommand);

#elif BDN_PLATFORM_WINRT

	int _uiAppMain(AppControllerBase* pAppController, Platform::Array<Platform::String^>^ args);

#elif BDN_PLATFORM_ANDROID
	// no main function

#else
	int _uiAppMain( AppControllerBase* pAppController,
					int argCount,
					char* argv[] );

#endif
	

}


/** \def BDN_INIT_UI_APP( appControllerClass )
 
    Sets up an app with a graphical user interface.
 
    appControllerClass is the class of the app's controller object. The app
    controller controls the startup and shutdown of your app and should
    be derived from AppControllerBase.
 
    Example:
 
    \code
 
    class MyAppController : public bdn::AppControllerBase
    {
    public:
        ... see AppControllerBase for functions to implement here
    };
 
    BDN_INIT_UI_APP( MyAppController );
 
    \endcode
 
 */
#if BDN_PLATFORM_WIN32

    // the main function must be WinMain. Also, we do not get commandlinnewe
	// arguments in a suitable form as a function argument (lpCmdLine is not
	// fully Unicode-compatible).

	#include <windows.h>

    #define BDN_INIT_UI_APP( appControllerClass )  \
		int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int showCommand) \
		{ \
			return bdn::_uiAppMain( bdn::newObj<appControllerClass>(), showCommand); \
		}

#elif BDN_PLATFORM_WINRT

	#define BDN_INIT_UI_APP( appControllerClass )  \
		int __cdecl main(Platform::Array<Platform::String^>^ args) \
		{ \
			return bdn::_uiAppMain( bdn::newObj<appControllerClass>(), args); \
		}

#elif BDN_PLATFORM_ANDROID

	#define BDN_INIT_UI_APP( appControllerClass )  \
		namespace bdn  \
		{  \
			namespace android  \
			{  \
				bdn::P<bdn::AppControllerBase> _createAppController() \
				{ \
					return bdn::newObj<appControllerClass>(); \
				}  \
				std::function<int(const AppLaunchInfo&)> _getAppFunc()  \
				{  \
					return std::function<int(const AppLaunchInfo&)>();
				} \
			}  \
		}

#else

    // standard main function

    #define BDN_INIT_UI_APP( appControllerClass ) \
		int main(int argc, char* argv[]) \
		{ \
			return bdn::_uiAppMain( bdn::newObj<appControllerClass>(), argc, argv ); \
		}

#endif



/** \def BDN_INIT_COMMANDLINE_APP( appFunc, appControllerClass = CommandLineAppController )
 
    Sets up a commandline app without a graphical user interface.

	appFunc is the main app function that does the actual work.
 
    \important You should NOT call the appFunc function "main", because Boden might
    generate a main function internally.
 
    appFunc must have the following interface:
 
    \code
    int myApp(const AppLaunchInfo& launchInfo)
    \endcode
 
    The launchInfo object contains the commandline arguments (and possibly other information).
	
    The return value of the appFunc is returned as the "exit code" of the program to
    the commandline caller. You should return 0 if the program executed successfully
    and an error code otherwise.
 
    \important Some systems limit exit codes to the range 0-255. You should only return
    values in that range for maximum compatibility.
 
    appControllerClass is an optional parameter to set the app's controller object. The app
    controller controls the startup and shutdown of your app and should
    be derived from AppControllerBase. The default here is CommandLineAppController, which
	provides an implementation that is suitable for most commandline apps.

	Graceful exit support
	============================

	If you want to enable graceful termination of your app (for example, so that the operating
	system can close it gracefully when the user requests it) then you should check the result
	of CommandLineAppController::shouldExit() periodically in your app and exit when it
	is set.

	If you need even more control over how a graceful exit is performed then you can provide 
	your own app controller class and override AppControllerBase::onTerminate() in it.
 
    Example: 
 
    \code

	#include <bdn/CommandLineAppController.h>
  
    int myApp(const AppLaunchInfo& launchInfo)
    {
        ... do stuff. 
		
		// periodically check if one should exit like this
		if( CommandLineAppController::shouldExit() )
			return 2;

		... do more stuff

		if( CommandLineAppController::shouldExit() )
			return 2;

		... do even more stuff
 
        return 0;
    }
 
    BDN_INIT_COMMANDLINE_APP( appFunc );
 
    \endcode
 
    */
#if BDN_PLATFORM_WINRT

	#define BDN_INIT_COMMANDLINE_APP( appFunc, ... )  \
		int main(Platform::Array<Platform::String^>^ args) \
		{ \
			return bdn::_commandLineAppMain(appFunc, bdn::_createCommandLineAppController<__VA_ARGS__>(), args); \
		}

#elif BDN_PLATFORM_ANDROID

	#define BDN_INIT_COMMANDLINE_APP( appFunc, ... )  \
		namespace bdn  \
		{  \
			namespace android  \
			{  \
				bdn::P<bdn::AppControllerBase> _createAppController() \
				{ \
					return bdn::_createCommandLineAppController<__VA_ARGS__>(); \
				}  \
				std::function<int(const AppLaunchInfo&)> _getAppFunc()  \
				{  \
					return appFunc;  \
				} \
			}  \
		}

#else

	#define BDN_INIT_COMMANDLINE_APP(appFunc, ... ) \
		int main(int argc, char* argv[]) \
		{ \
			return bdn::_commandLineAppMain(appFunc, bdn::_createCommandLineAppController<__VA_ARGS__>(), argc, argv); \
		}

#endif



/** \def BDN_INIT_COMMANDLINE_APP_WITH_UI( appControllerClass )
 
    Sets up a commandline app that can nevertheless have graphical user interface components.
	
	The internal structure of the app is the same as a normal UI app: an app controller
	controls the app and there is a normal event processing loop / message loop.

	See BDN_INIT_UI_APP() for more information about the parameters.

	The only difference to a normal UI app is, that the app is marked for the operating system to be
	a commandline app. On some systems this makes no difference at all. But on Windows, for example,
	this has the effect that the app will also automatically open a console window if it is run
	from a graphical app like Windows Explorer.
 
 */
#if BDN_PLATFORM_WIN32

	// the entry function must be called main on Windows as well.
	// But the _uiAppMain has a different signature on Windows, so we
	// must call it differently.

#define BDN_INIT_COMMANDLINE_APP_WITH_UI( appControllerClass ) \
		int main(int argc, char* argv[]) \
		{ \
			return bdn::_uiAppMain( bdn::newObj<appControllerClass>(), 1 /*SW_SHOWNORMAL*/ ); \
		}

#elif BDN_PLATFORM_WINRT

#define BDN_INIT_COMMANDLINE_APP_WITH_UI( appControllerClass )  \
		int main(Platform::Array<Platform::String^>^ args) \
		{ \
			return bdn::_uiAppMain( bdn::newObj<appControllerClass>(), args); \
		}


#elif BDN_PLATFORM_ANDROID

#define BDN_INIT_COMMANDLINE_APP_WITH_UI( appControllerClass )  \
		BDN_INIT_UI_APP(appControllerClass);

#else

#define BDN_INIT_COMMANDLINE_APP_WITH_UI( appControllerClass ) \
		int main(int argc, char* argv[]) \
		{ \
			return bdn::_uiAppMain( bdn::newObj<appControllerClass>(), argc, argv ); \
		}

#endif

		
/** \def BDN_INIT_COMMANDLINE_TEST_APP()

	Sets up a commandline app that runs the test cases defined with the boden test framework.	
	*/
#define BDN_INIT_COMMANDLINE_TEST_APP() BDN_INIT_COMMANDLINE_APP( bdn::_commandLineTestAppFunc );


/** \def BDN_INIT_UI_TEST_APP()

	Sets up a commandline test app that nevertheless supports a graphical user
	interface. The app will have a UI event loop / message loop.
	*/
#define BDN_INIT_UI_TEST_APP() BDN_INIT_COMMANDLINE_APP_WITH_UI( bdn::TestAppWithUiController );



/** \def BDN_INIT_INVISIBLE_APP( appFunc, appControllerClass )
 
    Sets up an app that is similar to a commandline app in its structure.
	However, it is intended to run invisibly and not interact with the user at all.
	It does not have a user interface (neither a graphical UI, nor a commandline interface).
 
    The main difference to a commandline app is that an invisible app will not take extra
	steps to show its output to the user. For example, on Windows systems normal commandline apps
	will automatically create a new commandline window if they are run from Windows Explorer or a
	graphical app. An invisible app will not do that.

    See #BDN_INIT_COMMANDLINE_APP() for more information on the parameters.
    */
#define BDN_INIT_INVISIBLE_APP( appFunc, appControllerClass )





#endif


