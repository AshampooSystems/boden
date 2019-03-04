
#import <bdn/mac/UIAppRunner.hh>
#import <bdn/mac/util.hh>

#import <bdn/foundationkit/MainDispatcher.hh>
#import <bdn/foundationkit/objectUtil.hh>

#include <bdn/ExceptionReference.h>
#include <bdn/entry.h>

#import <Cocoa/Cocoa.h>

@interface BdnMacAppDelegate_ : NSObject <NSApplicationDelegate>

- (void)setAppRunner:(bdn::mac::UIAppRunner *)runner;

@end

@implementation BdnMacAppDelegate_

bdn::mac::UIAppRunner *_runner;

- (void)setAppRunner:(bdn::mac::UIAppRunner *)runner { _runner = runner; }

- (void)applicationWillFinishLaunching:(NSNotification *)aNotification
{
    _runner->_applicationWillFinishLaunching(aNotification);
}

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification
{
    _runner->_applicationDidFinishLaunching(aNotification);
}

- (void)applicationDidBecomeActive:(NSNotification *)aNotification
{
    _runner->_applicationDidBecomeActive(aNotification);
}

- (void)applicationDidResignActive:(NSNotification *)aNotification
{
    _runner->_applicationDidResignActive(aNotification);
}

- (void)applicationWillTerminate:(NSNotification *)aNotification { _runner->_applicationWillTerminate(aNotification); }

@end

namespace bdn::mac
{
    AppLaunchInfo UIAppRunner::_makeLaunchInfo(int argCount, char *args[])
    {
        AppLaunchInfo launchInfo;

        std::vector<String> argStrings;
        for (int i = 0; i < argCount; i++)
            argStrings.push_back(args[i]);
        if (argCount == 0)
            argStrings.push_back(""); // always add the first entry.

        launchInfo.setArguments(argStrings);

        return launchInfo;
    }

    UIAppRunner::UIAppRunner(std::function<std::shared_ptr<AppControllerBase>()> appControllerCreator, int argCount,
                             char *args[])
        : AppRunnerBase(appControllerCreator, _makeLaunchInfo(argCount, args))
    {
        _mainDispatcher = std::make_shared<bdn::fk::MainDispatcher>();
    }

    static void _globalUnhandledNSException(NSException *exception)
    {
        NSObject *cppExceptionWrapper = nil;

        if (exception.userInfo != nil)
            cppExceptionWrapper = [exception.userInfo objectForKey:@"bdn::ExceptionReference"];

        std::shared_ptr<ExceptionReference> cppExceptionRef;
        if (cppExceptionWrapper != nil)
            cppExceptionRef =
                std::dynamic_pointer_cast<ExceptionReference>(bdn::fk::unwrapFromNSObject(cppExceptionWrapper));

        try {
            // if the exception is a wrapped C++ exception then we rethrow
            // the original
            if (cppExceptionRef != nullptr)
                cppExceptionRef->rethrow();
            else {
                // otherwise we throw the NSException pointer.
                throw exception;
            }
        }
        catch (...) {
            // note that exceptions are never recoverable on mac and ios
            bdn::unhandledException(false);
        }
    }

    bool UIAppRunner::isCommandLineApp() const { return false; }

    int UIAppRunner::entry()
    {
        NSSetUncaughtExceptionHandler(&_globalUnhandledNSException);

        [NSApplication sharedApplication];

        BdnMacAppDelegate_ *appDelegate = [[BdnMacAppDelegate_ alloc] init];
        [appDelegate setAppRunner:this];
        [NSApp setDelegate:appDelegate];
        [NSApp run];

        return 0;
    }

    void UIAppRunner::openURL(const String &url)
    {
        [[NSWorkspace sharedWorkspace] openURL:[NSURL URLWithString:stringToNSString(url)]];
    }

    void UIAppRunner::_applicationWillFinishLaunching(NSNotification *notification)
    {
        bdn::platformEntryWrapper(
            [&]() {
                prepareLaunch();
                beginLaunch();
            },
            false);
    }

    void UIAppRunner::_applicationDidFinishLaunching(NSNotification *notification)
    {
        bdn::platformEntryWrapper([&]() { finishLaunch(); }, false);
    }

    void UIAppRunner::_applicationDidBecomeActive(NSNotification *notification)
    {
        bdn::platformEntryWrapper([&]() { AppControllerBase::get()->onActivate(); }, false);
    }

    void UIAppRunner::_applicationDidResignActive(NSNotification *notification)
    {
        bdn::platformEntryWrapper([&]() { AppControllerBase::get()->onDeactivate(); }, false);
    }

    void UIAppRunner::_applicationWillTerminate(NSNotification *notification)
    {
        bdn::platformEntryWrapper([&]() { AppControllerBase::get()->onTerminate(); }, false);
    }

    void UIAppRunner::initiateExitIfPossible(int exitCode)
    {
        _mainDispatcher->enqueue([]() { [NSApp performSelector:@selector(terminate:) withObject:nil afterDelay:0.0]; });
    }

    void UIAppRunner::disposeMainDispatcher()
    {
        std::dynamic_pointer_cast<bdn::fk::MainDispatcher>(_mainDispatcher)->dispose();
    }
}
