#include <bdn/init.h>
#import <bdn/ios/WindowCore.hh>

#import <bdn/ios/MainViewController.hh>

namespace bdn
{
namespace ios
{


UIWindow* WindowCore::_createUIWindow(Window* pOuterWindow)
{
    UIWindow* window = [[UIWindow alloc] initWithFrame:[[UIScreen mainScreen] bounds]];
    
    MainViewController* controller = [MainViewController alloc];
    
    window.rootViewController = controller;
    
    window.backgroundColor = [UIColor whiteColor];
    
    // we must make this the key window here, before we do much else. Otherwise we
    // can get an app with a half-activated window that does not react to input events.
    // That happens even if makeKeyWindow or makeKeyAndVisible is called later.
    // We have to have one call right here.
    [window makeKeyAndVisible];
    
    return window;
}


}
}