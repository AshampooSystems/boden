#ifndef BDN_IOS_ButtonCore_HH_
#define BDN_IOS_ButtonCore_HH_

#include <bdn/IButtonCore.h>
#include <bdn/ClickEvent.h>
#include <bdn/Button.h>

#import <bdn/ios/ViewCore.hh>

namespace bdn
{
    namespace ios
    {

        class ButtonCore : public ViewCore, BDN_IMPLEMENTS IButtonCore
        {
          private:
            static UIButton *_createUIButton(Button *outerButton);

          public:
            ButtonCore(Button *outerButton);

            ~ButtonCore();

            UIButton *getUIButton() { return _button; }

            void setLabel(const String &label) override
            {
                [_button setTitle:stringToIosString(label) forState:UIControlStateNormal];
            }

            void _clicked();

          protected:
            double getFontSize() const override { return _button.titleLabel.font.pointSize; }

          private:
            UIButton *_button;

            NSObject *_clickManager;
        };
    }
}

#endif
