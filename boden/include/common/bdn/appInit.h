#ifndef BDN_appInit_H_
#define BDN_appInit_H_

#include <bdn/AppControllerBase.h>

#include <map>

namespace bdn
{
    
int _uiAppMain( AppControllerBase* pAppController,
                int argCount,
                char* argv[],
                const std::map<String,String>& launchInfo = std::map<String,String>() );
    
    
#if BDN_PLATFORM_IOS
    int _iosUiAppMain(AppControllerBase* pAppController,
                      int argCount,
                      char* argv[] );
    
#elif BDN_PLATFORM_OSX
    int _osxUiAppMain(AppControllerBase* pAppController,
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

    // the main function must have a different name

    #define BDN_INIT_UI_APP( appControllerClass )  \
    int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int showCommand) \
    { \
        return _uiAppMain(newObj<AppControllerClass>(), 0, nullptr, {"windows.showCommand", toString(showCommand) } ); \
    }

#else

    // standard main function

    #define BDN_INIT_UI_APP( appControllerClass ) \
    int main(int argc, char* argv[]) \
    { \
        return _uiAppMain(newObj<appControllerClass>(), argc, argv ); \
    }

#endif




/** \def BDN_INIT_COMMANDLINE_APP( appFunc, appControllerClass )
 
    Sets up a commandline app without a graphical user interface.
 
    appFunc is the main app function that does the actual work.
 
    \important You should NOT call the appFunc function "main", because Boden might
    generate a main function internally.
 
    appFunc must have the following interface:
 
    \code
    int myApp(const Array<String>& params)
    \endcode
 
    params is an array with the commandline parameters that the app got
    from its caller.
 
    \important The first item in \c params is actually the first commandline parameter,
    and NOT the name of the program. This is a difference to the parameters passed
    to the standard \c main function.
 
    The return value of the appFunc is returned as the "exit code" of the program to
    the commandline caller. You should return 0 if the program executed successfully
    and an error code otherwise.
 
    \important Some systems limit exit codes to the range 0-255. You should only return
    values in that range for maximum compatibility.
 
    appControllerClass is the class of the app's controller object. The app
    controller controls the startup and shutdown of your app and should
    be derived from AppControllerBase.
    The app controller is optional for commandline apps. If you do not need it
    you can simply pass AppControllerBase there. However, your should
    consider implementing at least the AppControllerBase::onTerminate handler,
    since it will enable the outside world to abort your app gracefully.
    But you should keep in mind that the AppControllerBase functions will be called
    from a different thread than the one that runs your appFunc function.
 
    Example without graceful termination support:
 
    \code
 
    int myApp(const Array<String>& params)
    {
        ... do stuff.
 
        return 0;
    }
 
    BDN_INIT_COMMANDLINE_APP( appFunc, AppControllerBase);
 
    \endcode
 
 
    Example with graceful termination support:
 
 
    \code
 
    class MyAppController : public bdn::AppControllerBase
    {
    public:
 
        void onTerminate()
        {
            ... set a flag to abort execution
        }
    };
 
    int myApp(const Array<String>& params)
    {
        ... do stuff. Abort when abort flag is set (see above)
 
        return 0;
    }
 
    BDN_INIT_COMMANDLINE_APP( appFunc, MyAppController );
 
    \endcode
 
    */
#define BDN_INIT_COMMANDLINE_APP(appFunc, appControllerClass )




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