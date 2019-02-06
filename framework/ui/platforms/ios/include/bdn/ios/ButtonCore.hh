#pragma once

#include <bdn/IButtonCore.h>
#include <bdn/ClickEvent.h>
#include <bdn/Button.h>

#import <bdn/ios/ViewCore.hh>

@class BdnIosButtonClickManager;

namespace bdn
{
    namespace ios
    {

        class ButtonCore : public ViewCore, virtual public IButtonCore
        {
          private:
            static UIButton *_createUIButton(std::shared_ptr<Button> outerButton);

          public:
            ButtonCore(std::shared_ptr<Button> outerButton);
            ~ButtonCore();

            UIButton *getUIButton();
            void setLabel(const String &label) override;

          protected:
            double getFontSize() const override;

          private:
            UIButton *_button;
            BdnIosButtonClickManager *_clickManager;
        };
    }
}
