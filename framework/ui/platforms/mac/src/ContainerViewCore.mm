#include <bdn/init.h>
#import <bdn/mac/ContainerViewCore.hh>

/** NSView implementation that is used internally by
 bdn::mac::ContainerViewCore.

 Sets the flipped property so that the coordinate system has its origin in the
 top left, rather than the bottom left.
 */
@interface BdnMacContainerView_ : NSView

@end

@implementation BdnMacContainerView_

- (BOOL)isFlipped { return YES; }

@end

namespace bdn
{
    namespace mac
    {

        NSView *ContainerViewCore::_createContainer(ContainerView *pOuter)
        {
            return [[BdnMacContainerView_ alloc] initWithFrame:NSMakeRect(0, 0, 0, 0)];
        }
    }
}