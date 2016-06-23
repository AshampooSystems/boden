
#import <Cocoa/Cocoa.h>

#include <bdn/AppControllerBase.h>

@interface AppDelegate : NSObject <NSApplicationDelegate>

    - (void)setAppController:(bdn::AppControllerBase*)pController;
    - (void)setLaunchInfo:(const bdn::AppLaunchInfo&)info;
        
@end

