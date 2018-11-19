#include <bdn/init.h>
#import <bdn/mac/UiAppRunner.hh>

#import <bdn/foundationkit/MainDispatcher.hh>
#import <bdn/foundationkit/objectUtil.hh>

#include <bdn/entry.h>
#include <bdn/ExceptionReference.h>

#import <Cocoa/Cocoa.h>

@interface BdnMacAppDelegate_ : NSObject <NSApplicationDelegate>

- (void)setAppRunner:(bdn::mac::UiAppRunner *)runner;

@end

@implementation BdnMacAppDelegate_

bdn::mac::UiAppRunner *_runner;

- (void)setAppRunner:(bdn::mac::UiAppRunner *)runner { _runner = runner; }

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

namespace bdn
{
    namespace mac
    {

        AppLaunchInfo UiAppRunner::_makeLaunchInfo(int argCount, char *args[])
        {
            AppLaunchInfo launchInfo;

            std::vector<String> argStrings;
            for (int i = 0; i < argCount; i++)
                argStrings.push_back(String::fromLocaleEncoding(args[i]));
            if (argCount == 0)
                argStrings.push_back(""); // always add the first entry.

            launchInfo.setArguments(argStrings);

            return launchInfo;
        }

        UiAppRunner::UiAppRunner(std::function<P<AppControllerBase>()> appControllerCreator, int argCount, char *args[])
            : AppRunnerBase(appControllerCreator, _makeLaunchInfo(argCount, args))
        {
            _mainDispatcher = newObj<bdn::fk::MainDispatcher>();
        }

        static void _globalUnhandledNSException(NSException *exception)
        {
            NSObject *cppExceptionWrapper = nil;

            if (exception.userInfo != nil)
                cppExceptionWrapper = [exception.userInfo objectForKey:@"bdn::ExceptionReference"];

            P<ExceptionReference> cppExceptionRef;
            if (cppExceptionWrapper != nil)
                cppExceptionRef = tryCast<ExceptionReference>(bdn::fk::unwrapFromNSObject(cppExceptionWrapper));

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

        bool UiAppRunner::isCommandLineApp() const { return false; }

        int UiAppRunner::entry()
        {
            NSSetUncaughtExceptionHandler(&_globalUnhandledNSException);

            [NSApplication sharedApplication];

            BdnMacAppDelegate_ *appDelegate = [[BdnMacAppDelegate_ alloc] init];
            [appDelegate setAppRunner:this];
            [NSApp setDelegate:appDelegate];
            [NSApp run];

            return 0;
        }

        void UiAppRunner::_applicationWillFinishLaunching(NSNotification *notification)
        {
            bdn::platformEntryWrapper(
                [&]() {
                    prepareLaunch();
                    beginLaunch();
                },
                false);
        }

        void UiAppRunner::_applicationDidFinishLaunching(NSNotification *notification)
        {
            bdn::platformEntryWrapper([&]() { finishLaunch(); }, false);
        }

        void UiAppRunner::_applicationDidBecomeActive(NSNotification *notification)
        {
            bdn::platformEntryWrapper([&]() { AppControllerBase::get()->onActivate(); }, false);
        }

        void UiAppRunner::_applicationDidResignActive(NSNotification *notification)
        {
            bdn::platformEntryWrapper([&]() { AppControllerBase::get()->onDeactivate(); }, false);
        }

        void UiAppRunner::_applicationWillTerminate(NSNotification *notification)
        {
            bdn::platformEntryWrapper([&]() { AppControllerBase::get()->onTerminate(); }, false);
        }

        void UiAppRunner::initiateExitIfPossible(int exitCode)
        {
            _mainDispatcher->enqueue(
                []() { [NSApp performSelector:@selector(terminate:) withObject:nil afterDelay:0.0]; });
        }

        void UiAppRunner::disposeMainDispatcher() { cast<bdn::fk::MainDispatcher>(_mainDispatcher)->dispose(); }
    }
}
