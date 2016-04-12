#import <Cocoa/Cocoa.h>

#include <bdn/init.h>
#include <bdn/appInit.h>

#include "AppDelegate.h"

namespace bdn
{

int _osxUiAppMain(AppControllerBase* pAppController, int argc, char * argv[])
{
    [NSApplication sharedApplication];
    
    AppDelegate* appDelegate = [[AppDelegate alloc] init];
    [appDelegate setAppController:pAppController];
    [NSApp setDelegate:appDelegate];
    [NSApp run];
    
    return 0;
}

}

