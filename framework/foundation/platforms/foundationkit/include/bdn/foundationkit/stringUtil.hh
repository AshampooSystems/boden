#ifndef BDN_FK_stringUtil_HH_
#define BDN_FK_stringUtil_HH_

#import <Foundation/Foundation.h>

namespace bdn
{
    namespace fk
    {

        inline NSString *stringToNSString(const String &val)
        {
            return [NSString stringWithCString:val.asUtf8Ptr()
                                      encoding:NSUTF8StringEncoding];
        }

        inline String nsStringToString(NSString *nsString)
        {
            return [nsString cStringUsingEncoding:NSUTF8StringEncoding];
        }
    }
}

#endif
