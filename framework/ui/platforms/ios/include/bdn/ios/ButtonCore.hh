#pragma once

#include <bdn/Button.h>
#include <bdn/ClickEvent.h>
#include <bdn/IButtonCore.h>

#import <bdn/ios/ViewCore.hh>

@class BdnIosButtonClickManager;

namespace bdn::ios
{
    class ButtonCore : public ViewCore, virtual public IButtonCore
    {
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
