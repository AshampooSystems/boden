
#import <bdn/mac/ContainerViewCore.hh>

/** NSView implementation that is used internally by
 bdn::mac::ContainerViewCore.

 Sets the flipped property so that the coordinate system has its origin in the
 top left, rather than the bottom left.
 */
@interface BdnMacContainerView_ : NSView
@property bdn::ContainerView *containerView;
@end

@implementation BdnMacContainerView_

- (BOOL)isFlipped { return YES; }

- (void)layout
{
    if (_containerView) {
        _containerView->getLayout()->layout(_containerView);
    }
}

@end

namespace bdn::mac
{
    NSView *ContainerViewCore::_createContainer(std::shared_ptr<ContainerView> outer)
    {
        BdnMacContainerView_ *macContainerView = [[BdnMacContainerView_ alloc] initWithFrame:NSMakeRect(0, 0, 0, 0)];

        macContainerView.containerView = outer.get();
        return macContainerView;
    }
}
