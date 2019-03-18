#pragma once

#include <bdn/Button.h>
#include <bdn/ButtonCore.h>
#include <bdn/ClickEvent.h>

#import <bdn/ios/ViewCore.hh>

@class BdnIosButtonClickManager;

namespace bdn::ios
{
    class ButtonCore : public ViewCore, virtual public bdn::ButtonCore
    {
      public:
        ButtonCore(const std::shared_ptr<bdn::UIProvider> &uiProvider);
        ~ButtonCore() override;

        UIButton *getUIButton();
        void handleClick();

      protected:
        double getFontSize() const override;

      private:
        UIButton *_button;
        BdnIosButtonClickManager *_clickManager;
    };
}
