#pragma once

#include <bdn/String.h>

#pragma once

#include <bdn/Color.h>
#include <bdn/Font.h>
#include <bdn/platform.h>

#ifdef BDN_PLATFORM_IOS
#import <UIKit/UIKit.h>
#else
#import <AppKit/AppKit.h>
#endif

#import <Foundation/Foundation.h>

namespace bdn::fk
{

#ifdef BDN_PLATFORM_IOS
    UIColor *_Nullable colorToFkColor(const bdn::Color &color, UIColor *_Nullable defaultColor = nullptr);
    UIFont *_Nullable fontToFkFont(const bdn::Font &font, UIFont *_Nullable defaultFont = nullptr);

#else
    NSFont *_Nullable fontToFkFont(const bdn::Font &font, NSFont *_Nullable defaultFont = nullptr);
    NSColor *_Nullable colorToFkColor(const bdn::Color &color, NSColor *_Nullable defaultColor = nullptr);

#endif

    inline NSString *_Nullable stringToNSString(const String &val)
    {
        if (val.empty()) {
            return [NSString string];
        }
        NSString *result = [NSString stringWithCString:val.c_str() encoding:NSUTF8StringEncoding];
        if (result == nullptr) {
            return [NSString string];
        }
        return result;
    }

    inline String nsStringToString(NSString *_Nullable nsString)
    {
        if (nsString != nullptr) {
            return [nsString cStringUsingEncoding:NSUTF8StringEncoding];
        }
        return String();
    }
}
