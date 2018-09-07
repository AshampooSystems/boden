#include <bdn/init.h>

#import <Cocoa/Cocoa.h>

#import <bdn/mac/ButtonCore.hh>

@interface BdnButtonClickManager : NSObject

@property bdn::mac::ButtonCore *pButtonCore;

@end

@implementation BdnButtonClickManager

- (void)setButtonCore:(bdn::mac::ButtonCore *)pCore { _pButtonCore = pCore; }

- (void)clicked { _pButtonCore->generateClick(); }

@end

namespace bdn
{
    namespace mac
    {

        ButtonCore::ButtonCore(Button *pOuterButton)
            : ButtonCoreBase(pOuterButton, _createNsButton(pOuterButton))
        {
            _currBezelStyle = NSRoundedBezelStyle;

            BdnButtonClickManager *clickMan = [BdnButtonClickManager alloc];
            [clickMan setButtonCore:this];
            _clickManager = clickMan;

            setLabel(pOuterButton->label());

            _heightWithRoundedBezelStyle =
                macSizeToSize(getNSView().fittingSize).height;

            [_nsButton setTarget:clickMan];
            [_nsButton setAction:@selector(clicked)];
        }

        void ButtonCore::_updateBezelStyle()
        {
            Size size;

            P<View> pView = getOuterViewIfStillAttached();
            if (pView != nullptr)
                size = pView->size();
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
                bezelStyle = NSRegularSquareBezelStyle;
            else
                bezelStyle = NSRoundedBezelStyle;

            if (bezelStyle != _currBezelStyle) {
                [_nsButton setBezelStyle:bezelStyle];
            }
        }

        void ButtonCore::generateClick()
        {
            P<Button> pOuterButton =
                cast<Button>(getOuterViewIfStillAttached());
            if (pOuterButton != nullptr) {
                bdn::ClickEvent evt(pOuterButton);

                pOuterButton->onClick().notify(evt);
            }
        }
    }
}
