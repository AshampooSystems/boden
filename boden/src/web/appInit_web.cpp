#include <bdn/init.h>
#include <bdn/appInit.h>

#include <bdn/Thread.h>
#include <bdn/Uri.h>


namespace bdn
{
    

int _uiAppMain( AppControllerBase* pAppController,
                int argCount,
                char* argv[] )
{
    _mainInit();
    
        
    // init app launch info
    
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

    AppControllerBase::_set(pAppController);


    pAppController->beginLaunch( launchInfo );
    pAppController->finishLaunch( launchInfo );

    // just exit. The emscripten compile flags we use ensure that the app keeps
    // running.
    return 0;
}
    

}



