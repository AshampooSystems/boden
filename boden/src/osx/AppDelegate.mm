#include <bdn/init.h>
#include <bdn/AppControllerBase.h>

#import "AppDelegate.h"


using namespace bdn;


@implementation AppDelegate

    std::map<bdn::String,bdn::String>   _launchInfo;

- (void)setAppController:(bdn::AppControllerBase*) pController {
    AppControllerBase::get() = pController;
}
                          
                          
- (void)applicationWillFinishLaunching:(NSNotification *)aNotification {
    AppControllerBase::get()->beginLaunch(_launchInfo);
}


- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
    AppControllerBase::get()->finishLaunch(_launchInfo);
}
                          
                          
- (void)applicationDidBecomeActive:(NSNotification *)aNotification {
    AppControllerBase::get()->onActivate();
}

                          
- (void)applicationDidResignActive:(NSNotification *)aNotification {
    AppControllerBase::get()->onDeactivate();
}
                          

- (void)applicationWillTerminate:(NSNotification *)aNotification {
    AppControllerBase::get()->onTerminate();
    
    AppControllerBase::_set(nullptr);
}
                          
                          
                          


@end
