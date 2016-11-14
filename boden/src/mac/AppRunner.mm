#include <bdn/init.h>
#import <bdn/mac/AppRunner.hh>

#import <bdn/mac/Dispatcher.hh>

#import <Cocoa/Cocoa.h>

#include <bdn/entry.h>


@interface AppDelegate : NSObject <NSApplicationDelegate>

- (void)setAppRunner:(bdn::mac::AppRunner*)pRunner;

@end


@implementation AppDelegate

bdn::mac::AppRunner* _pRunner;

- (void)setAppRunner:(bdn::mac::AppRunner*) pRunner {
    _pRunner = pRunner;
}


- (void)applicationWillFinishLaunching:(NSNotification *)aNotification {
    BDN_ENTRY_BEGIN;
    _pRunner->_applicationWillFinishLaunching(aNotification);
    
    BDN_ENTRY_END(false);
}


- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
    BDN_ENTRY_BEGIN;
    _pRunner->_applicationDidFinishLaunching(aNotification);
    BDN_ENTRY_END(false);
}


- (void)applicationDidBecomeActive:(NSNotification *)aNotification {
    BDN_ENTRY_BEGIN;
    _pRunner->_applicationDidBecomeActive(aNotification);
    BDN_ENTRY_END(true);
}


- (void)applicationDidResignActive:(NSNotification *)aNotification {
    BDN_ENTRY_BEGIN;
    _pRunner->_applicationDidResignActive(aNotification);
    BDN_ENTRY_END(true);
}


- (void)applicationWillTerminate:(NSNotification *)aNotification {
    BDN_ENTRY_BEGIN;
    _pRunner->_applicationWillTerminate(aNotification);
    BDN_ENTRY_END(false);
}


@end


namespace bdn
{
namespace mac
{

AppLaunchInfo AppRunner::_makeLaunchInfo(int argCount, char* args[] )
{
    AppLaunchInfo launchInfo;
    
    std::vector<String> argStrings;
    for(int i=0; i<argCount; i++)
        argStrings.push_back( String::fromLocaleEncoding(args[i]) );
    if(argCount==0)
        argStrings.push_back("");	// always add the first entry.

    launchInfo.setArguments(argStrings);
    
    return launchInfo;
}

AppRunner::AppRunner( std::function< P<AppControllerBase>() > appControllerCreator, int argCount, char* args[])
    : AppRunnerBase(appControllerCreator, _makeLaunchInfo(argCount, args) )
{
    _pDispatcher = newObj<Dispatcher>();
}


int AppRunner::entry()
{
    [NSApplication sharedApplication];
    
    AppDelegate* appDelegate = [[AppDelegate alloc] init];
    [appDelegate setAppRunner:this];
    [NSApp setDelegate:appDelegate];
    [NSApp run];
    
    return 0;
}


void AppRunner::_applicationWillFinishLaunching(NSNotification* notification)
{
    prepareLaunch();
    beginLaunch();
}

void AppRunner::_applicationDidFinishLaunching(NSNotification* notification)
{
    finishLaunch();
}

void AppRunner::_applicationDidBecomeActive(NSNotification* notification)
{
    AppControllerBase::get()->onActivate();
}

void AppRunner::_applicationDidResignActive(NSNotification* notification)
{
    AppControllerBase::get()->onDeactivate();
}

void AppRunner::_applicationWillTerminate(NSNotification* notification)
{
    AppControllerBase::get()->onTerminate();
}



}
}

