#include <bdn/init.h>
#import <bdn/UiKitFrame.hh>

#import <bdn/MainViewController.h>

namespace bdn
{
    
    
P<IFrame> createFrame(const String& title)
{
    return newObj<UiKitFrame>(title);
}



UiKitFrame::UiKitFrame(const String& title)
{
    UIWindow* window = [[UIWindow alloc] initWithFrame:[[UIScreen mainScreen] bounds]];
    
    MainViewController* controller = [MainViewController alloc];
    window.rootViewController = controller;
    
    window.backgroundColor = [UIColor whiteColor];
    //controller.view.backgroundColor = [UIColor whiteColor];
    //controller.title = @"Hello";
    
    // we must make this the key window here, before we do much else. Otherwise we
    // can get an app with a half-activated window that does not react to input events.
    // That happens even if makeKeyWindow or makeKeyAndVisible is called later.
    // We have to have one call right here.
    [window makeKeyAndVisible];
    
    initWindow(window, title);
}
    
}

