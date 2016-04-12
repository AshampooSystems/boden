//
//  AppDelegate.h
//  hello
//
//  Created by Hauke Duden on 19/07/2015.
//  Copyright (c) 2015 Hauke Duden. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#include <bdn/AppControllerBase.h>

@interface AppDelegate : NSObject <NSApplicationDelegate>

    - (void)setAppController:(bdn::AppControllerBase*)pController;
        
@end

