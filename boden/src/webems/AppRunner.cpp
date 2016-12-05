#include <bdn/init.h>
#include <bdn/webems/UiAppRunner.h>

namespace bdn
{
namespace webems
{
    
AppLaunchInfo AppRunner::_makeLaunchInfo(int argCount, char* args[] )
{
    AppLaunchInfo launchInfo;

    std::vector<String> argStrings;
    for(int i=0; i<argCount; i++)
        argStrings.push_back( String::fromLocaleEncoding(args[i]) );
    if(argCount==0)
        argStrings.push_back(""); // always add the first entry.
    launchInfo.setArguments(argStrings);

    return launchInfo;
}


AppRunner::AppRunner( std::function< P<AppControllerBase>() > appControllerCreator, int argCount, char* args[])
 : AppRunnerBase(appControllerCreator, _makeLaunchInfo(argCount, args) )
{        
}

void AppRunner::entry()
{
    _pMainDispatcher = newObj<MainDispatcher>();
    
    launch();
}

void AppRunner::initiateExitIfPossible(int exitCode)
{
    AppControllerBase::get()->onTerminate();

    emscripten_force_exit(exitCode);
}

void UiAppRunner::disposeMainDispatcher()
{
    _pMainDispatcher->dispose();
}
    


}
}



