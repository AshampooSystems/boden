#include <bdn/init.h>
#include <bdn/gtk/UiAppRunner.h>

#include <bdn/mainThread.h>

namespace bdn
{
namespace gtk
{
    
AppLaunchInfo UiAppRunner::_makeLaunchInfo(int argCount, char* args[] )
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


UiAppRunner::UiAppRunner( std::function< P<AppControllerBase>() > appControllerCreator, int argCount, char* args[])
 : AppRunnerBase(appControllerCreator, _makeLaunchInfo(argCount, args) )
{        
}

int UiAppRunner::entry()
{
    gtk_init(NULL, NULL);
    
    _pMainDispatcher = newObj<MainDispatcher>();
    
    launch();
    
    gtk_main();

    terminating();

    {
        MutexLock lock(_exitMutex);

        return _exitCode;
    }
}

void UiAppRunner::initiateExitIfPossible(int exitCode)
{
    P<UiAppRunner> pThis = this;
    
    callFromMainThread(
		[pThis, exitCode]()
		{
            {
                MutexLock lock(pThis->_exitMutex);
                pThis->_exitCode = exitCode;
            }
            
			gtk_main_quit();
		} );
}

void UiAppRunner::disposeMainDispatcher()
{
    _pMainDispatcher->dispose();
}
    


}
}



