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
        ButtonCore(const std::shared_ptr<bdn::ViewCoreFactory> &viewCoreFactory);
        ~ButtonCore() override;

        UIButton *getUIButton();
        void handleClick();

      private:
        UIButton *_button;
        BdnIosButtonClickManager *_clickManager;
    };
}
