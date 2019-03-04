
#import <bdn/ios/AppRunner.hh>
#import <bdn/ios/util.hh>

#import <bdn/foundationkit/MainDispatcher.hh>
#import <bdn/foundationkit/objectUtil.hh>

#include <bdn/ExceptionReference.h>
#include <bdn/entry.h>

#include <bdn/AppControllerBase.h>

#import <UIKit/UIKit.h>

@interface BdnIosAppDelegate_ : UIResponder <UIApplicationDelegate>

@property(strong, nonatomic) UIWindow *window;

// static method
+ (void)setStaticAppRunner:(bdn::ios::AppRunner *)runner;

@end

@implementation BdnIosAppDelegate_

static bdn::ios::AppRunner *_staticAppRunner;
bdn::ios::AppRunner *_appRunner;

- (id)init
{
    self = [super init];
    if (self) {
        _appRunner = _staticAppRunner;
    }
    return self;
}

+ (void)setStaticAppRunner:(bdn::ios::AppRunner *)runner { _staticAppRunner = runner; }

- (BOOL)application:(UIApplication *)application willFinishLaunchingWithOptions:(NSDictionary *)launchOptions
{
    return _appRunner->_applicationWillFinishLaunching(launchOptions) ? YES : NO;
}

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions
{
    return _appRunner->_applicationDidFinishLaunching(launchOptions) ? YES : NO;
}

- (void)applicationDidBecomeActive:(UIApplication *)application
{
    _appRunner->_applicationDidBecomeActive(application);
}

- (void)applicationWillResignActive:(UIApplication *)application
{
    _appRunner->_applicationWillResignActive(application);
}

- (void)applicationDidEnterBackground:(UIApplication *)application
{
    _appRunner->_applicationDidEnterBackground(application);
}

- (void)applicationWillEnterForeground:(UIApplication *)application
{
    _appRunner->_applicationWillEnterForeground(application);
}

- (void)applicationWillTerminate:(UIApplication *)application { _appRunner->_applicationWillTerminate(application); }

@end

namespace bdn::ios
{
    AppLaunchInfo AppRunner::_makeLaunchInfo(int argCount, char *args[])
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

    AppRunner::AppRunner(const std::function<std::shared_ptr<AppControllerBase>()> &appControllerCreator, int argCount,
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

    bool AppRunner::isCommandLineApp() const
    {
        // iOS does not support commandline apps.
        return false;
    }

    int AppRunner::entry(int argCount, char *args[])
    {
        NSSetUncaughtExceptionHandler(&_globalUnhandledNSException);

        [BdnIosAppDelegate_ setStaticAppRunner:this];

        @autoreleasepool {
            return UIApplicationMain(argCount, args, nil, NSStringFromClass([BdnIosAppDelegate_ class]));
        }
    }

    void AppRunner::openURL(const String &url)
    {
        [[UIApplication sharedApplication] openURL:[NSURL URLWithString:stringToNSString(url)]
                                           options:@{}
                                 completionHandler:nil];
    }

    bool AppRunner::_applicationWillFinishLaunching(NSDictionary *launchOptions)
    {
        bdn::platformEntryWrapper(
            [&]() {
                prepareLaunch();
                beginLaunch();
            },
            false);

        return true;
    }

    bool AppRunner::_applicationDidFinishLaunching(NSDictionary *launchOptions)
    {
        bdn::platformEntryWrapper([&]() { finishLaunch(); }, false);

        return true;
    }

    void AppRunner::_applicationDidBecomeActive(UIApplication *application)
    {
        bdn::platformEntryWrapper([&]() { AppControllerBase::get()->onActivate(); }, false);
    }

    void AppRunner::_applicationWillResignActive(UIApplication *application)
    {
        bdn::platformEntryWrapper([&]() { AppControllerBase::get()->onDeactivate(); }, false);
    }

    void AppRunner::_applicationDidEnterBackground(UIApplication *application) {}

    void AppRunner::_applicationWillEnterForeground(UIApplication *application) {}

    void AppRunner::_applicationWillTerminate(UIApplication *application)
    {
        bdn::platformEntryWrapper([&]() { AppControllerBase::get()->onTerminate(); }, false);
    }

    void AppRunner::initiateExitIfPossible(int exitCode)
    {
        // ios apps cannot close themselves. So we do nothing here.
        int x = 0;
        x++;
    }

    void AppRunner::disposeMainDispatcher()
    {
        std::dynamic_pointer_cast<bdn::fk::MainDispatcher>(_mainDispatcher)->dispose();
    }
}
