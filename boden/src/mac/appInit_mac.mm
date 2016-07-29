#import <Cocoa/Cocoa.h>

#include <bdn/init.h>
#include <bdn/appInit.h>
#include <bdn/Thread.h>

#import "AppDelegate.hh"

namespace bdn
{

int _uiAppMain(AppControllerBase* pAppController, int argCount, char* argv[])
{
    Thread::_setMainId( Thread::getCurrentId() );
    
    
    // init app launch info
    
    AppLaunchInfo       launchInfo;
    std::vector<String> args;
    
    for(int i=0; i<argCount; i++)
        args.push_back( String::fromLocaleEncoding(argv[i]) );
    if(argCount==0)
        args.push_back("");	// always add the first entry.
    
    launchInfo.setArguments(args);

    
    // just launch the app. The app delegate will take care of calling
    // the proper methods of the boden app controller.
    
    [NSApplication sharedApplication];
    
    AppDelegate* appDelegate = [[AppDelegate alloc] init];
    [appDelegate setAppController:pAppController];
    [appDelegate setLaunchInfo:launchInfo];
    [NSApp setDelegate:appDelegate];
    [NSApp run];
    
    return 0;
}

}

