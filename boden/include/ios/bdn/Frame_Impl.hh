#ifndef _BDN_FRAME_IMPL_H
#define _BDN_FRAME_IMPL_H

#include <bdn/Frame.h>

#import <UIKit/UIKit.h>
#import "MainViewController.h"


namespace bdn
{

class Frame::Impl
{
public:
    Impl(const String& title)
    {
        _window = [[UIWindow alloc] initWithFrame:[[UIScreen mainScreen] bounds]];
        
        MainViewController* controller = [MainViewController alloc];
        _window.rootViewController = controller;
        
        _window.backgroundColor = [UIColor whiteColor];
        //controller.view.backgroundColor = [UIColor whiteColor];
        //controller.title = @"Hello";
        
        [_window makeKeyAndVisible];
    }
    
    UIWindow* getWindow()
    {
        return _window;
    }

    
    void show(bool visible)
    {
        _window.hidden = visible ? FALSE : TRUE;
        
        if(visible)
            [_window makeKeyAndVisible];        
    }
    
protected:
    UIWindow* _window;
};

}

#endif