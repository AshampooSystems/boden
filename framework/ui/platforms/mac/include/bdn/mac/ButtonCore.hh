#pragma once

#include <bdn/ui/Button.h>

#import <bdn/mac/ViewCore.hh>

@class BdnButtonClickManager;

namespace bdn::ui::mac
{
    class ButtonCore : public ViewCore, virtual public Button::Core
    {
      private:
        static NSButton *_createNsButton();

      public:
        ButtonCore(const std::shared_ptr<ViewCoreFactory> &viewCoreFactory);
        ~ButtonCore();

        void init() override;

        Size sizeForSpace(Size availableSpace) const override;
        float calculateBaseline(Size forSize, bool forIndicator) const override;

        void frameChanged() override;

      public:
        void handleClick();

      protected:
        void setFrame(Rect r) override;

      private:
        void _updateBezelStyle();
        void updateText(const Text &text);
        void updateImage(const std::string &url);

      private:
        BdnButtonClickManager *_clickManager;

        NSBezelStyle _currBezelStyle;
        int _heightWithRoundedBezelStyle{};
    };
}
