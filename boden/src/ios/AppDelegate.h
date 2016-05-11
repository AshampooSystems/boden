
#import <UIKit/UIKit.h>

#include <bdn/AppControllerBase.h>

@interface AppDelegate : UIResponder <UIApplicationDelegate>

@property (strong, nonatomic) UIWindow *window;

// static method
+ (void)setStaticLaunchInfo:(const bdn::AppLaunchInfo&)info;


@end

