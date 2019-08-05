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

      public:
        float calculateBaseline(Size forSize) const override;
        float pointScaleFactor() const override;

      private:
        void textChanged(const Text &text);
        void imageChanged(const std::string &url);

      private:
        UIButton *_button;
        BdnIosButtonClickManager *_clickManager;
    };
}
