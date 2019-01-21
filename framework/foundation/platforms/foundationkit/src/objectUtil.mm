
#import <bdn/foundationkit/objectUtil.hh>

#import <CoreFoundation/CoreFoundation.h>

/** Wraps an IBase pointer into an NSObject.*/
@interface BdnFkNSObjectWrapper_ : NSObject

- (void)setObject:(std::shared_ptr<bdn::Base>)object;

- (std::shared_ptr<bdn::Base>)getObject;

@end

@implementation BdnFkNSObjectWrapper_

std::shared_ptr<bdn::Base> _object;

- (void)setObject:(std::shared_ptr<bdn::Base>)object { _object = object; }

- (std::shared_ptr<bdn::Base>)getObject { return _object; }

@end

namespace bdn
{
    namespace fk
    {

        NSObject *wrapIntoNSObject(std::shared_ptr<Base> p)
        {
            BdnFkNSObjectWrapper_ *wrapper = [BdnFkNSObjectWrapper_ alloc];
            [wrapper setObject:p];

            return wrapper;
        }

        std::shared_ptr<bdn::Base> unwrapFromNSObject(NSObject *ns)
        {
            if (ns != nil && [ns isKindOfClass:[BdnFkNSObjectWrapper_ class]]) {
                std::shared_ptr<bdn::Base> p([(BdnFkNSObjectWrapper_ *)ns getObject]);
                return p;
            }

            return nullptr;
        }
    }
}
