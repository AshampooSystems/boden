#pragma once

#include <bdn/String.h>

#import <Foundation/Foundation.h>

namespace bdn
{
    namespace fk
    {

        inline NSString *stringToNSString(const String &val)
        {
            return [NSString stringWithCString:val.c_str() encoding:NSUTF8StringEncoding];
        }

        inline String nsStringToString(NSString *nsString)
        {
            return [nsString cStringUsingEncoding:NSUTF8StringEncoding];
        }
    }
}
