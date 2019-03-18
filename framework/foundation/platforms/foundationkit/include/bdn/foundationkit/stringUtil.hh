#pragma once

#include <bdn/String.h>

#import <Foundation/Foundation.h>

namespace bdn
{
    namespace fk
    {
        inline NSString *_Nonnull stringToNSString(const String &val)
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
}
