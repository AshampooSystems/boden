#include <bdn/init.h>
#import <bdn/ios/ButtonCore.hh>

@interface BdnIosButtonClickManager : NSObject

@property bdn::ios::ButtonCore *core;

@end

@implementation BdnIosButtonClickManager

- (void)setButtonCore:(bdn::ios::ButtonCore *)core { _core = core; }

- (void)clicked { _core->_clicked(); }

@end

namespace bdn
{
    namespace ios
    {

        UIButton *ButtonCore::_createUIButton(Button *outerButton)
        {
            UIButton *button = [UIButton buttonWithType:UIButtonTypeSystem];

            return button;
        }

        ButtonCore::ButtonCore(Button *outerButton) : ViewCore(outerButton, _createUIButton(outerButton))
        {
            _button = (UIButton *)getUIView();

            BdnIosButtonClickManager *clickMan = [BdnIosButtonClickManager alloc];
            [clickMan setButtonCore:this];

            _clickManager = clickMan;

            [_button addTarget:clickMan action:@selector(clicked) forControlEvents:UIControlEventTouchUpInside];

            setLabel(outerButton->label());
        }

        ButtonCore::~ButtonCore()
        {
            BdnIosButtonClickManager *clickMan = (BdnIosButtonClickManager *)_clickManager;

            [_button removeTarget:clickMan action:nil forControlEvents:UIControlEventTouchUpInside];
        }

        void ButtonCore::_clicked()
        {
            P<View> view = getOuterViewIfStillAttached();
            if (view != nullptr) {
                ClickEvent evt(view);

                cast<Button>(view)->onClick().notify(evt);
            }
        }
    }
}
