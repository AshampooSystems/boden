#pragma once

#include <bdn/ui/Button.h>
#include <bdn/ui/ClickEvent.h>

#import <bdn/ios/ViewCore.hh>

@class BdnIosButtonClickManager;

namespace bdn::ui::ios
{
    class ButtonCore : public ViewCore, virtual public ui::Button::Core
    {
      public:
        ButtonCore(const std::shared_ptr<ViewCoreFactory> &viewCoreFactory);
        ~ButtonCore();

        UIButton *getUIButton();
        void handleClick();

      private:
        UIButton *_button;
        BdnIosButtonClickManager *_clickManager;
    };
}
