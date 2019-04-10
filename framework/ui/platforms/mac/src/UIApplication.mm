
#import <bdn/mac/UIApplication.hh>
#import <bdn/mac/util.hh>

#import <bdn/foundationkit/MainDispatcher.hh>
#include <utility>

#include <bdn/entry.h>

#import <Cocoa/Cocoa.h>

@interface BdnMacAppDelegate_ : NSObject <NSApplicationDelegate>
@property(nonatomic) std::weak_ptr<bdn::mac::UIApplication> bdnApplication;
@end

@implementation BdnMacAppDelegate_

bdn::mac::UIApplication *_runner;

- (void)applicationWillFinishLaunching:(NSNotification *)aNotification
{
    if (auto app = self.bdnApplication.lock()) {
        app->_applicationWillFinishLaunching(aNotification);
    }
}

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification
{
    if (auto app = self.bdnApplication.lock()) {
        app->_applicationDidFinishLaunching(aNotification);
    }
}

- (void)applicationDidBecomeActive:(NSNotification *)aNotification
{
    if (auto app = self.bdnApplication.lock()) {
        app->_applicationDidBecomeActive(aNotification);
    }
}

- (void)applicationDidResignActive:(NSNotification *)aNotification
{
    if (auto app = self.bdnApplication.lock()) {
        app->_applicationDidResignActive(aNotification);
    }
}

- (void)applicationWillTerminate:(NSNotification *)aNotification
{
    if (auto app = self.bdnApplication.lock()) {
        app->_applicationWillTerminate(aNotification);
    }
}

@end

namespace bdn::mac
{
    UIApplication::UIApplication(Application::ApplicationControllerFactory appControllerCreator, int argCount,
                                 char *args[])
        : Application(std::move(appControllerCreator), std::make_shared<bdn::fk::MainDispatcher>())
    {
        buildCommandlineArguments(argCount, args);
    }

    int UIApplication::entry()
    {
        [NSApplication sharedApplication];

        BdnMacAppDelegate_ *appDelegate = [[BdnMacAppDelegate_ alloc] init];
        appDelegate.bdnApplication = std::dynamic_pointer_cast<bdn::mac::UIApplication>(shared_from_this());
        [NSApp setDelegate:appDelegate];
        [NSApp run];

        return 0;
    }

    void UIApplication::openURL(const String &url)
    {
        NSURL *nsUrl = [NSURL URLWithString:fk::stringToNSString(url)];
        if (nsUrl != nullptr) {
            [[NSWorkspace sharedWorkspace] openURL:nsUrl];
        }
    }

    void UIApplication::_applicationWillFinishLaunching(NSNotification *notification)
    {
        bdn::platformEntryWrapper(
            [&]() {
                prepareLaunch();
                beginLaunch();
            },
            false);
    }

    void UIApplication::_applicationDidFinishLaunching(NSNotification *notification)
    {
        bdn::platformEntryWrapper([&]() { finishLaunch(); }, false);
    }

    void UIApplication::_applicationDidBecomeActive(NSNotification *notification)
    {
        bdn::platformEntryWrapper([&]() { applicationController()->onActivate(); }, false);
    }

    void UIApplication::_applicationDidResignActive(NSNotification *notification)
    {
        bdn::platformEntryWrapper([&]() { applicationController()->onDeactivate(); }, false);
    }

    void UIApplication::_applicationWillTerminate(NSNotification *notification)
    {
        bdn::platformEntryWrapper([&]() { applicationController()->onTerminate(); }, false);
    }

    void UIApplication::initiateExitIfPossible(int exitCode)
    {
        dispatchQueue()->dispatchAsync(
            []() { [NSApp performSelector:@selector(terminate:) withObject:nil afterDelay:0.0]; });
    }

    void UIApplication::disposeMainDispatcher()
    {
        std::dynamic_pointer_cast<bdn::fk::MainDispatcher>(dispatchQueue())->dispose();
    }

    void UIApplication::buildCommandlineArguments(int argCount, char *args[])
    {
        std::vector<String> argStrings;
        argStrings.reserve(argCount);
        for (int i = 0; i < argCount; i++) {
            // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
            argStrings.emplace_back(args[i]);
        }
        if (argCount == 0) {
            argStrings.emplace_back(""); // always add the first entry.
        }

        _argc = argCount;
        _argv = (char **)args;

        commandLineArguments = argStrings;
    }
}
