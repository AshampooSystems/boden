#include <bdn/init.h>
#import <bdn/foundationkit/objectUtil.hh>

#import <CoreFoundation/CoreFoundation.h>

/** Wraps an IBase pointer into an NSObject.*/
@interface BdnFkNSObjectWrapper_ : NSObject

- (void)setObject:(bdn::IBase *)object;

- (bdn::IBase *)getObject;

@end

@implementation BdnFkNSObjectWrapper_

bdn::P<bdn::IBase> _object;

- (void)setObject:(bdn::IBase *)object { _object = object; }

- (bdn::IBase *)getObject { return _object; }

@end

namespace bdn
{
    namespace fk
    {

        NSObject *wrapIntoNSObject(IBase *p)
        {
            BdnFkNSObjectWrapper_ *wrapper = [BdnFkNSObjectWrapper_ alloc];
            [wrapper setObject:p];

            return wrapper;
        }

        P<IBase> unwrapFromNSObject(NSObject *ns)
        {
            if (ns != nil && [ns isKindOfClass:[BdnFkNSObjectWrapper_ class]]) {
                P<IBase> p([(BdnFkNSObjectWrapper_ *)ns getObject]);
                return p;
            }

            return nullptr;
        }
    }
}
