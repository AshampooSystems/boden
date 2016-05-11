#include <bdn/init.h>
#include <bdn/AppControllerBase.h>

#import "AppDelegate.h"

using namespace bdn;


@implementation AppDelegate

    static AppLaunchInfo    _staticLaunchInfo;
    AppLaunchInfo           _launchInfo;

- (id)init
{
    self = [super init];
    if (self)
    {
        _launchInfo = _staticLaunchInfo;
    }
    return self;
}


+ (void)setStaticLaunchInfo:(const bdn::AppLaunchInfo&) info {
    
    _staticLaunchInfo = info;
}


- (BOOL)application:(UIApplication *)application willFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    AppControllerBase::get()->beginLaunch(_launchInfo);
    return YES;
}


- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    AppControllerBase::get()->finishLaunch(_launchInfo);
    return YES;
}


- (void)applicationDidBecomeActive:(UIApplication *)application {
    AppControllerBase::get()->onActivate();
}


- (void)applicationWillResignActive:(UIApplication *)application {
    AppControllerBase::get()->onDeactivate();
}


- (void)applicationDidEnterBackground:(UIApplication *)application {
    AppControllerBase::get()->onSuspend();
}

- (void)applicationWillEnterForeground:(UIApplication *)application {
    AppControllerBase::get()->onResume();
}


- (void)applicationWillTerminate:(UIApplication *)application {
    AppControllerBase::get()->onTerminate();
    
    AppControllerBase::_set(nullptr);
}

@end
