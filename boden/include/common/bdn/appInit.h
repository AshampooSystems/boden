#ifndef BDN_appInit_H_
#define BDN_appInit_H_

#include <bdn/AppControllerBase.h>
#include <bdn/CommandLineAppController.h>
#include <bdn/TestAppWithUiController.h>

#include <map>
#include <vector>

namespace bdn
{
    



int _commandLineAppMain(	std::function< int(const std::vector<String>&) > appFunc,
							AppControllerBase* pAppController,
							int argCount,
							char* argv[] );


template<class ControllerType = CommandLineAppController>
inline P<ControllerType> _createCommandLineAppController()
{
	return newObj<ControllerType>();	
}
    

int _commandLineTestAppFunc( const std::vector<String>& params );


#if BDN_PLATFORM_WINDOWS_CLASSIC
	int _uiAppMain(AppControllerBase* pAppController, int showCommand);

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
#if BDN_PLATFORM_WINDOWS_CLASSIC

    // the main function must be WinMain. Also, we do not get commandlinnewe
	// arguments in a suitable form as a function argument (lpCmdLine is not
	// fully Unicode-compatible).

	#include <windows.h>

    #define BDN_INIT_UI_APP( appControllerClass )  \
		int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int showCommand) \
		{ \
			bdn::_uiAppMain( bdn::newObj<appControllerClass>(), showCommand); \
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
    int myApp(const Array<String>& args)
    \endcode
 
    args is an array with the commandline arguments that the app got
    from its caller.

	The first item of args is a representation of the program name and/or path.
	You should not depend on this first entry, though. It is highly dependent on the platform
	and on how the app was called. Sometimes the first entry can be a full path. Sometimes it
	is a relative path or just a file name. On some platforms it can also simply be an empty string.
 

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
  
    int myApp(const Array<String>& args)
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
#define BDN_INIT_COMMANDLINE_APP(appFunc, ... ) \
		int main(int argc, char* argv[]) \
		{ \
			bdn::_commandLineAppMain(appFunc, bdn::_createCommandLineAppController<__VA_ARGS__>(), argc, argv); \
		}

		
/** \def BDN_INIT_COMMANDLINE_TEST_APP()

	Sets up a commandline app that runs the test cases defined with the boden test framework.	
	*/
#define BDN_INIT_COMMANDLINE_TEST_APP() BDN_INIT_COMMANDLINE_APP( bdn::_commandLineTestAppFunc );


/** \def BDN_INIT_UI_TEST_APP()

	Sets up a test app with a graphical user interface that runs 
	runs the test cases defined with the boden test framework.	
	*/
#define BDN_INIT_UI_TEST_APP() BDN_INIT_UI_APP( bdn::TestAppWithUiController );



/** \def BDN_INIT_INVISIBLE_APP( appFunc, appControllerClass )
 
    Sets up an app that is intended to run invisibly. It does not have a user interface
    (neither a graphical UI, nor a commandline interface).
 
    On some systems the implementation is similar to a commandline app.
    The main difference is that an invisible app will not take extra steps to show its
    output to the user on systems that do not support traditional commandline apps.
 
    See #BDN_INIT_COMMANDLINE_APP() for more information on the parameters.
    */
#define BDN_INIT_INVISIBLE_APP( appFunc, appControllerClass )





#endif