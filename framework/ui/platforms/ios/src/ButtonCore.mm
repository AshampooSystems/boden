
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

        UIButton *ButtonCore::_createUIButton(std::shared_ptr<Button> outerButton)
        {
            return [UIButton buttonWithType:UIButtonTypeSystem];
        }

        ButtonCore::ButtonCore(std::shared_ptr<Button> outerButton)
            : ViewCore(outerButton, _createUIButton(outerButton))
        {
            _button = (UIButton *)getUIView();

            BdnIosButtonClickManager *clickMan = [BdnIosButtonClickManager alloc];
            [clickMan setButtonCore:this];

            _clickManager = clickMan;

            [_button addTarget:clickMan action:@selector(clicked) forControlEvents:UIControlEventTouchUpInside];

            setLabel(outerButton->label);
        }

        ButtonCore::~ButtonCore()
        {
            BdnIosButtonClickManager *clickMan = (BdnIosButtonClickManager *)_clickManager;

            [_button removeTarget:clickMan action:nil forControlEvents:UIControlEventTouchUpInside];
        }

        UIButton *ButtonCore::getUIButton() { return _button; }

        void ButtonCore::setLabel(const String &label)
        {
            [_button setTitle:stringToIosString(label) forState:UIControlStateNormal];
        }

        void ButtonCore::_clicked()
        {
            std::shared_ptr<View> view = getOuterViewIfStillAttached();
            if (view != nullptr) {
                ClickEvent evt(view);

                std::dynamic_pointer_cast<Button>(view)->onClick().notify(evt);
            }
        }

        double ButtonCore::getFontSize() const { return _button.titleLabel.font.pointSize; }
    }
}
