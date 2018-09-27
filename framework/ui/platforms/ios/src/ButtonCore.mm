#include <bdn/init.h>
#import <bdn/ios/ButtonCore.hh>

@interface BdnIosButtonClickManager : NSObject

@property bdn::ios::ButtonCore *pCore;

@end

@implementation BdnIosButtonClickManager

- (void)setButtonCore:(bdn::ios::ButtonCore *)pCore { _pCore = pCore; }

- (void)clicked { _pCore->_clicked(); }

@end

namespace bdn
{
    namespace ios
    {

        UIButton *ButtonCore::_createUIButton(Button *pOuterButton)
        {
            UIButton *button = [UIButton buttonWithType:UIButtonTypeSystem];

            return button;
        }

        ButtonCore::ButtonCore(Button *pOuterButton)
            : ViewCore(pOuterButton, _createUIButton(pOuterButton))
        {
            _button = (UIButton *)getUIView();

            BdnIosButtonClickManager *clickMan =
                [BdnIosButtonClickManager alloc];
            [clickMan setButtonCore:this];

            _clickManager = clickMan;

            [_button addTarget:clickMan
                          action:@selector(clicked)
                forControlEvents:UIControlEventTouchUpInside];

            setLabel(pOuterButton->label());
        }

        ButtonCore::~ButtonCore()
        {
            BdnIosButtonClickManager *clickMan =
                (BdnIosButtonClickManager *)_clickManager;

            [_button removeTarget:clickMan
                           action:nil
                 forControlEvents:UIControlEventTouchUpInside];
        }

        void ButtonCore::_clicked()
        {
            P<View> pView = getOuterViewIfStillAttached();
            if (pView != nullptr) {
                ClickEvent evt(pView);

                cast<Button>(pView)->onClick().notify(evt);
            }
        }
    }
}
