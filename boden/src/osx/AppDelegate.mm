//
//  AppDelegate.m
//  hello
//
//  Created by Hauke Duden on 19/07/2015.
//  Copyright (c) 2015 Hauke Duden. All rights reserved.
//

#import "AppDelegate.h"

#include <bdn/App.h>

@interface AppDelegate ()

@end

@implementation AppDelegate

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
    
    bdn::App* pApp = bdn::App::get();
    pApp->init();
}


- (void)applicationWillTerminate:(NSNotification *)aNotification {
    // Insert code here to tear down your application
}


@end
