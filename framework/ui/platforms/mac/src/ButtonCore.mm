

#import <Cocoa/Cocoa.h>

#import <bdn/mac/ButtonCore.hh>

@interface BdnButtonClickManager : NSObject

@property(nonatomic, assign) std::weak_ptr<bdn::Button> outer;

@end

@implementation BdnButtonClickManager

- (void)clicked
{
    if (auto outer = self.outer.lock()) {
        bdn::ClickEvent clickEvent(outer);
        outer->onClick().notify(clickEvent);
    }
}

@end

namespace bdn
{
    namespace mac
    {

        ButtonCore::ButtonCore(std::shared_ptr<Button> outer) : ButtonCoreBase(outer, _createNsButton(outer))
        {
            _currBezelStyle = NSBezelStyleRounded;

            _clickManager = [[BdnButtonClickManager alloc] init];
            _clickManager.outer = outer;

            setLabel(outer->label);

            _heightWithRoundedBezelStyle = macSizeToSize(getNSView().fittingSize).height;

            [_nsButton setTarget:_clickManager];
            [_nsButton setAction:@selector(clicked)];
        }

        void ButtonCore::_updateBezelStyle()
        {
            Size size;

            std::shared_ptr<View> view = getOuterViewIfStillAttached();
            if (view != nullptr)
                size = view->size;
            int height = size.height;

            // the "normal" button (NSRoundedBezelStyle) has a fixed height.
            // If we want a button that is higher then we have to use another
            // bezel style, which will look somewhat "non-standard" because it
            // has different highlights.

            // we switch to the non-standard bezel style when our height
            // "significantly" exceeds the fixed height we get with the standard
            // bezel.
            NSBezelStyle bezelStyle;

            if (height > _heightWithRoundedBezelStyle * 1.1)
                bezelStyle = NSBezelStyleRegularSquare;
            else
                bezelStyle = NSBezelStyleRounded;

            if (bezelStyle != _currBezelStyle) {
                [_nsButton setBezelStyle:bezelStyle];
            }
        }
    }
}
