#pragma once

#include <bdn/Button.h>
#include <bdn/ClickEvent.h>

#import <bdn/ios/ViewCore.hh>

@class BdnIosButtonClickManager;

namespace bdn::ios
{
    class ButtonCore : public ViewCore, virtual public bdn::Button::Core
    {
      public:
        ButtonCore(const std::shared_ptr<bdn::ViewCoreFactory> &viewCoreFactory);
        ~ButtonCore();

        UIButton *getUIButton();
        void handleClick();

      private:
        UIButton *_button;
        BdnIosButtonClickManager *_clickManager;
    };
}
