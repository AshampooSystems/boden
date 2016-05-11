

#include <bdn/init.h>
#include <bdn/appInit.h>
#include <bdn/Thread.h>

#include "AppDelegate.h"

namespace bdn
{

int _uiAppMain(AppControllerBase* pAppController, int argCount, char* argv[])
{
    Thread::_setMainId( Thread::getCurrentId() );
    
    AppControllerBase::_set(pAppController);
    
    
    // init app launch info
    
    AppLaunchInfo       launchInfo;
    std::vector<String> args;
    
    for(int i=0; i<argCount; i++)
        args.push_back( String::fromLocaleEncoding(argv[i]) );
    if(argCount==0)
        args.push_back("");	// always add the first entry.
    
    launchInfo.setArguments(args);
    
    [AppDelegate setStaticLaunchInfo:launchInfo];

    
    @autoreleasepool
    {
        return UIApplicationMain(argCount, argv, nil, NSStringFromClass([AppDelegate class]));
    }
}


}

