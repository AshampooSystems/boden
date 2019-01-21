#pragma once

#import <CoreFoundation/CoreFoundation.h>
#include <memory>

#include <bdn/Base.h>

namespace bdn
{
    namespace fk
    {

        /** Wraps a Boden object (derived from bdn::Base or implementing
           bdn::IBase) into an NSObject, suitable for passing to iOS or macOS
           routines.

            The object can be unwrapped with unwrapFromNSObject()

            */
        NSObject *wrapIntoNSObject(std::shared_ptr<Base> p);

        /** Unwraps a previously wrapped Boden object from an NSObject.
            If the specified NSObject is nil or was not created by
           wrapIntoNSObject then this function returns null pointer.
            */
        std::shared_ptr<Base> unwrapFromNSObject(NSObject *ns);
    }
}
