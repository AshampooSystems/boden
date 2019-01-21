

#import <Cocoa/Cocoa.h>

#import <bdn/mac/ButtonCore.hh>

@interface BdnButtonClickManager : NSObject

@property bdn::mac::ButtonCore *buttonCore;

@end

@implementation BdnButtonClickManager

- (void)clicked { _buttonCore->generateClick(); }

@end

namespace bdn
{
    namespace mac
    {

        ButtonCore::ButtonCore(std::shared_ptr<Button> outerButton)
            : ButtonCoreBase(outerButton, _createNsButton(outerButton))
        {
            _currBezelStyle = NSBezelStyleRounded;

            BdnButtonClickManager *clickMan = [BdnButtonClickManager alloc];
            [clickMan setButtonCore:this];
            _clickManager = clickMan;

            setLabel(outerButton->label);

            _heightWithRoundedBezelStyle = macSizeToSize(getNSView().fittingSize).height;

            [_nsButton setTarget:clickMan];
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

        void ButtonCore::generateClick()
        {
            std::shared_ptr<Button> outerButton = std::dynamic_pointer_cast<Button>(getOuterViewIfStillAttached());
            if (outerButton != nullptr) {
                bdn::ClickEvent evt(outerButton);

                outerButton->onClick().notify(evt);
            }
        }
    }
}
