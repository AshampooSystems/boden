
#import <bdn/mac/UIAppRunner.hh>
#import <bdn/mac/util.hh>

#import <bdn/foundationkit/MainDispatcher.hh>
#import <bdn/foundationkit/objectUtil.hh>
#include <utility>

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
        argStrings.reserve(argCount);
        for (int i = 0; i < argCount; i++) {
            // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
            argStrings.emplace_back(args[i]);
        }
        if (argCount == 0) {
            argStrings.emplace_back(""); // always add the first entry.
        }

        launchInfo.setArguments(argStrings);

        return launchInfo;
    }

    UIAppRunner::UIAppRunner(std::function<std::shared_ptr<ApplicationController>()> appControllerCreator, int argCount,
                             char *args[])
        : AppRunnerBase(std::move(appControllerCreator), _makeLaunchInfo(argCount, args))
    {
        _mainDispatcher = std::make_shared<bdn::fk::MainDispatcher>();
    }

    bool UIAppRunner::isCommandLineApp() const { return false; }

    int UIAppRunner::entry()
    {
        [NSApplication sharedApplication];

        BdnMacAppDelegate_ *appDelegate = [[BdnMacAppDelegate_ alloc] init];
        [appDelegate setAppRunner:this];
        [NSApp setDelegate:appDelegate];
        [NSApp run];

        return 0;
    }

    void UIAppRunner::openURL(const String &url)
    {
        NSURL *nsUrl = [NSURL URLWithString:fk::stringToNSString(url)];
        if (nsUrl != nullptr) {
            [[NSWorkspace sharedWorkspace] openURL:nsUrl];
        }
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
        bdn::platformEntryWrapper([&]() { ApplicationController::get()->onActivate(); }, false);
    }

    void UIAppRunner::_applicationDidResignActive(NSNotification *notification)
    {
        bdn::platformEntryWrapper([&]() { ApplicationController::get()->onDeactivate(); }, false);
    }

    void UIAppRunner::_applicationWillTerminate(NSNotification *notification)
    {
        bdn::platformEntryWrapper([&]() { ApplicationController::get()->onTerminate(); }, false);
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
