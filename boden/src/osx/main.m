//
//  main.m
//  hello
//
//  Created by Hauke Duden on 19/07/2015.
//  Copyright (c) 2015 Hauke Duden. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#include "AppDelegate.h"

int main(int argc, const char * argv[])
{
    [NSApplication sharedApplication];
    
    AppDelegate* appDelegate = [[AppDelegate alloc] init];
    [NSApp setDelegate:appDelegate];
    [NSApp run];
    
    return 0;
}
