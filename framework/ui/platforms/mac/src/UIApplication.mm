#import <bdn/mac/UIApplication.hh>

#include <bdn/entry.h>
#include <bdn/log.h>
#include <bdn/ui/View.h>
#include <utility>

#import <Cocoa/Cocoa.h>
#import <bdn/foundationkit/MainDispatcher.hh>
#import <bdn/foundationkit/conversionUtil.hh>
#import <bdn/mac/util.hh>

@interface BdnMacAppDelegate_ : NSObject <NSApplicationDelegate>
@property(nonatomic) std::weak_ptr<bdn::ui::mac::UIApplication> bdnApplication;
@end

@implementation BdnMacAppDelegate_

bdn::ui::mac::UIApplication *_runner;

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

namespace bdn::ui::mac
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
        appDelegate.bdnApplication = std::dynamic_pointer_cast<mac::UIApplication>(shared_from_this());
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

    String UIApplication::uriToBundledFileUri(const String &uri)
    {
        String result;
        std::regex re("(resource|asset|image)://([^/]*)/(.*)");

        std::smatch base_match;
        std::regex_match(uri, base_match, re);

        if (!base_match.empty()) {
            std::string scheme = base_match[1];
            std::string host = base_match[2];
            std::string path = base_match[3];

            NSBundle *bundle = [NSBundle mainBundle];
            if (!host.empty() && host != "main") {
                bundle = [NSBundle bundleWithIdentifier:fk::stringToNSString(host)];
            }
            if (bundle) {
                if (scheme == "resource" || scheme == "asset") {
                    auto lastSeperator = path.find_last_of('/');
                    if (lastSeperator) {
                        auto [directory, filename] = bdn::path::split(path);
                        auto [basename, ext] = bdn::path::splitExt(filename);

                        if (scheme == "asset") {
                            directory = "assets/" + directory;
                        }

                        result = fk::nsStringToString([bundle pathForResource:fk::stringToNSString(basename)
                                                                       ofType:fk::stringToNSString(ext)
                                                                  inDirectory:fk::stringToNSString(directory)]);
                    }
                } else if (scheme == "image") {
                    auto [basename, ext] = bdn::path::splitExt(path);

                    String fixedPath;
                    fixedPath.resize(path.size());

                    std::transform(path.begin(), path.end(), fixedPath.begin(),
                                   [](auto c) { return c == '/' || c == '.' ? '_' : c; });

                    result = fk::nsStringToString([bundle pathForImageResource:fk::stringToNSString(fixedPath)]);
                }
            }
        }

        if (result.empty()) {
            return result;
        }

        return "file:///" + result;
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

        if (std::find(argStrings.begin(), argStrings.end(), "--bdn-view-enable-debug"s) != argStrings.end()) {
            View::debugViewEnabled() = true;
        }

        if (std::find(argStrings.begin(), argStrings.end(), "--bdn-view-enable-baseline-debug"s) != argStrings.end()) {
            View::debugViewBaselineEnabled() = true;
        }
    }
}
