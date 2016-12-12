#include <bdn/init.h>
#include <bdn/webems/AppRunner.h>

#include <bdn/Uri.h>

#include <emscripten/emscripten.h>

namespace bdn
{
namespace webems
{
    
AppLaunchInfo AppRunner::_makeLaunchInfo(int argCount, char* argv[] )
{
    AppLaunchInfo       launchInfo;
    std::vector<String> args;
    
    for(int i=0; i<argCount; i++)
    {
        // arguments are URL-escaped.
        args.push_back( Uri::unescape( String(argv[i]) ) );
    }
    if(argCount==0)
        args.push_back(""); // always add the first entry.
    
    launchInfo.setArguments(args);

    return launchInfo;
}


AppRunner::AppRunner( std::function< P<AppControllerBase>() > appControllerCreator, int argCount, char* args[])
 : AppRunnerBase(appControllerCreator, _makeLaunchInfo(argCount, args) )
{        
}

int AppRunner::entry()
{
    _pMainDispatcher = newObj<MainDispatcher>();
    
    launch();

    // just exit. The emscripten compile flags we use ensure that the app keeps
    // running.
    // By exiting we pass control to the browser, which will run the event loop.
    return 0;
}

void AppRunner::initiateExitIfPossible(int exitCode)
{
    AppControllerBase::get()->onTerminate();

    emscripten_force_exit(exitCode);
}

void AppRunner::disposeMainDispatcher()
{
    _pMainDispatcher->dispose();
}
    


}
}



