#include <bdn/init.h>
#include <bdn/mac/UiAppRunner.h>

#import <bdn/fk/MainDispatcher.hh>
#import <bdn/fk/objectUtil.hh>

#include <bdn/entry.h>
#include <bdn/ExceptionReference.h>

#import <Cocoa/Cocoa.h>




@interface BdnMacAppDelegate_ : NSObject <NSApplicationDelegate>

- (void)setAppRunner:(bdn::mac::UiAppRunner*)pRunner;

@end


@implementation BdnMacAppDelegate_

bdn::mac::UiAppRunner* _pRunner;

- (void)setAppRunner:(bdn::mac::UiAppRunner*) pRunner {
    _pRunner = pRunner;
}


- (void)applicationWillFinishLaunching:(NSNotification *)aNotification
{
    _pRunner->_applicationWillFinishLaunching();
}


- (void)applicationDidFinishLaunching:(NSNotification *)aNotification
{
    _pRunner->_applicationDidFinishLaunching();
}


- (void)applicationDidBecomeActive:(NSNotification *)aNotification
{
    _pRunner->_applicationDidBecomeActive();
}


- (void)applicationDidResignActive:(NSNotification *)aNotification
{
    _pRunner->_applicationDidResignActive();
}


- (void)applicationWillTerminate:(NSNotification *)aNotification
{
    _pRunner->_applicationWillTerminate();
}


@end


namespace bdn
{
namespace mac
{

AppLaunchInfo UiAppRunner::_makeLaunchInfo(int argCount, char* args[] )
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

UiAppRunner::UiAppRunner( std::function< P<AppControllerBase>() > appControllerCreator, int argCount, char* args[])
    : AppRunnerBase(appControllerCreator, _makeLaunchInfo(argCount, args) )
{
    _pMainDispatcher = newObj<bdn::fk::MainDispatcher>();
}




static void _globalUnhandledNSException(NSException* exception)
{
    NSObject* cppExceptionWrapper = nil;
    
    // XXX
    std::cerr << "global handler called" << std::endl;
    
    if(exception.userInfo!=nil)
        cppExceptionWrapper = [exception.userInfo objectForKey:@"bdn::ExceptionReference"];
    
    P<ExceptionReference> pCppExceptionRef;
    if(cppExceptionWrapper!=nil)
        pCppExceptionRef = tryCast<ExceptionReference>( bdn::fk::unwrapFromNSObject( cppExceptionWrapper ) );
    
    try
    {
        // if the exception is a wrapped C++ exception then we rethrow
        // the original
        if(pCppExceptionRef!=nullptr)
        {
            // XXX
            std::cerr << "rethrow c++" << std::endl;
            pCppExceptionRef->rethrow();
        }
        else
        {
            // otherwise we throw the NSException pointer.

            // XXX
            std::cerr << "rethrow ns" << std::endl;
            
            throw exception;
        }
    }
    catch(...)
    {
        // XXX
        std::cerr << "recaught: calling unhandledException" << std::endl;


        // note that exceptions are never recoverable on mac and ios
        bdn::unhandledException(false);
    }
}

int UiAppRunner::entry()
{
    NSSetUncaughtExceptionHandler( &_globalUnhandledNSException );

    [NSApplication sharedApplication];
    
    BdnMacAppDelegate_* appDelegate = [[BdnMacAppDelegate_ alloc] init];
    [appDelegate setAppRunner:this];
    [NSApp setDelegate:appDelegate];
    [NSApp run];
    
    return 0;
}


void UiAppRunner::_applicationWillFinishLaunching()
{
    BDN_ENTRY_BEGIN;

    prepareLaunch();
    beginLaunch();
    
    BDN_ENTRY_END(false);
}

void UiAppRunner::_applicationDidFinishLaunching()
{
    BDN_ENTRY_BEGIN;
    
    finishLaunch();
    
    BDN_ENTRY_END(false);
}

void UiAppRunner::_applicationDidBecomeActive()
{
    BDN_ENTRY_BEGIN;
    
    AppControllerBase::get()->onActivate();

    BDN_ENTRY_END(false);
}

void UiAppRunner::_applicationDidResignActive()
{
    BDN_ENTRY_BEGIN;
    
    AppControllerBase::get()->onDeactivate();
    
    BDN_ENTRY_END(false);
}

void UiAppRunner::_applicationWillTerminate()
{
    BDN_ENTRY_BEGIN;
    
    AppControllerBase::get()->onTerminate();

    BDN_ENTRY_END(false);
}

void UiAppRunner::initiateExitIfPossible(int exitCode)
{
    _pMainDispatcher->enqueue(
        []()
        {
            [NSApp performSelector:@selector(terminate:) withObject:nil afterDelay:0.0];
        } );
}


void UiAppRunner::disposeMainDispatcher()
{
    cast<bdn::fk::MainDispatcher>(_pMainDispatcher)->dispose();
}



    
    

}
}

