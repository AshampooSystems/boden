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

      public:
        void handleClick();

      private:
        void _updateBezelStyle();

        BdnButtonClickManager *_clickManager;

        NSBezelStyle _currBezelStyle;
        int _heightWithRoundedBezelStyle{};
    };
}
