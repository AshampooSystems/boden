#ifndef BDN_FK_objectUtil_HH_
#define BDN_FK_objectUtil_HH_

#import <CoreFoundation/CoreFoundation.h>

namespace bdn
{
namespace fk
{


/** Wraps a Boden object (derived from bdn::Base or implementing bdn::IBase)
    into an NSObject, suitable for passing to iOS or macOS routines.
    
    The object can be unwrapped with unwrapFromNSObject()
    
    */
NSObject* wrapIntoNSObject(IBase* p);


/** Unwraps a previously wrapped Boden object from an NSObject.
    If the specified NSObject is nil or was not created by wrapIntoNSObject then
    this function returns null pointer.
    */
P<IBase> unwrapFromNSObject(NSObject* ns);


}
}

#endif
