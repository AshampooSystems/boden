
#import <bdn/foundationkit/MainDispatcher.hh>
#import <bdn/foundationkit/stringUtil.hh>
#import <bdn/ios/UIApplication.hh>

#include <bdn/ApplicationController.h>
#include <bdn/entry.h>
#include <bdn/ui/View.h>

#import <UIKit/UIKit.h>

@interface BdnIosAppDelegate_ : UIResponder <UIApplicationDelegate>
@property(strong, nonatomic) UIWindow *window;
@property(nonatomic) std::weak_ptr<bdn::ui::ios::UIApplication> bdnApplication;
@end

@implementation BdnIosAppDelegate_

static std::weak_ptr<bdn::ui::ios::UIApplication> s_staticApplication;

- (id)init
{
    self = [super init];
    if (self != nullptr) {
        self.bdnApplication = s_staticApplication;
    }
    return self;
}

- (BOOL)application:(UIApplication *)application willFinishLaunchingWithOptions:(NSDictionary *)launchOptions
{
    if (auto app = self.bdnApplication.lock()) {
        return app->_applicationWillFinishLaunching(launchOptions) ? YES : NO;
    }
    return false;
}

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions
{
    if (auto app = self.bdnApplication.lock()) {
        return app->_applicationDidFinishLaunching(launchOptions) ? YES : NO;
    }
    return NO;
}

- (void)applicationDidBecomeActive:(UIApplication *)application
{
    if (auto app = self.bdnApplication.lock()) {
        app->_applicationDidBecomeActive();
    }
}

- (void)applicationWillResignActive:(UIApplication *)application
{
    if (auto app = self.bdnApplication.lock()) {
        app->_applicationWillResignActive();
    }
}

- (void)applicationDidEnterBackground:(UIApplication *)application
{
    if (auto app = self.bdnApplication.lock()) {
        app->_applicationDidEnterBackground();
    }
}

- (void)applicationWillEnterForeground:(UIApplication *)application
{
    if (auto app = self.bdnApplication.lock()) {
        app->_applicationWillEnterForeground();
    }
}

- (void)applicationWillTerminate:(UIApplication *)application
{
    if (auto app = self.bdnApplication.lock()) {
        app->_applicationWillTerminate();
    }
}

@end

namespace bdn::ui::ios
{
    UIApplication::UIApplication(const std::function<std::shared_ptr<ApplicationController>()> &appControllerCreator,
                                 int argCount, char *args[])
        : Application(appControllerCreator, std::make_shared<bdn::fk::MainDispatcher>())
    {
        buildCommandlineArguments(argCount, args);
    }

    int UIApplication::entry(int argCount, char *args[])
    {
        s_staticApplication = std::dynamic_pointer_cast<UIApplication>(shared_from_this());

        @autoreleasepool {
            return UIApplicationMain(argCount, args, nil, NSStringFromClass([BdnIosAppDelegate_ class]));
        }
    }

    void UIApplication::openURL(const String &url)
    {
        if (auto app = [::UIApplication sharedApplication]) {
            if (auto nsUrl = [NSURL URLWithString:fk::stringToNSString(url)]) {
                [app openURL:nsUrl options:@{} completionHandler:nil];
            }
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
                // if (scheme == "resource" || scheme == "asset") {
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
                //}
            }
        }

        if (result.empty()) {
            return result;
        }

        return "file:///" + result;
    }

    bool UIApplication::_applicationWillFinishLaunching(NSDictionary *launchOptions)
    {
        bdn::platformEntryWrapper(
            [&]() {
                prepareLaunch();
                beginLaunch();
            },
            false);

        return true;
    }

    bool UIApplication::_applicationDidFinishLaunching(NSDictionary *launchOptions)
    {
        bdn::platformEntryWrapper([&]() { finishLaunch(); }, false);

        return true;
    }

    void UIApplication::_applicationDidBecomeActive()
    {
        bdn::platformEntryWrapper([&]() { applicationController()->onActivate(); }, false);
    }

    void UIApplication::_applicationWillResignActive()
    {
        bdn::platformEntryWrapper([&]() { applicationController()->onDeactivate(); }, false);
    }

    void UIApplication::_applicationDidEnterBackground() {}

    void UIApplication::_applicationWillEnterForeground() {}

    void UIApplication::_applicationWillTerminate()
    {
        bdn::platformEntryWrapper([&]() { applicationController()->onTerminate(); }, false);
    }

    void UIApplication::initiateExitIfPossible(int exitCode) {}

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
        _argv = args;

        commandLineArguments = argStrings;

        if (std::find(argStrings.begin(), argStrings.end(), "--bdn-view-enable-debug"s) != argStrings.end()) {
            View::debugViewEnabled() = true;
        }
        if (std::find(argStrings.begin(), argStrings.end(), "--bdn-view-enable-baseline-debug"s) != argStrings.end()) {
            View::debugViewBaselineEnabled() = true;
        }
    }
}
