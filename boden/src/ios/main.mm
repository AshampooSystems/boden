#import <UIKit/UIKit.h>

#include <bdn/init.h>
#include <bdn/appInit.h>

#include "AppDelegate.h"

namespace bdn
{
    
int _iosUiAppMain(AppControllerBase* pAppController, int argc, char* argv[])
{
    @autoreleasepool {
        return UIApplicationMain(argc, argv, nil, NSStringFromClass([AppDelegate class]));
    }
}
    
    
}
