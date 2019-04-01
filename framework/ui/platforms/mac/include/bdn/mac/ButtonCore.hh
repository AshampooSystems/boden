#pragma once

#include <bdn/Button.h>

#import <bdn/mac/ViewCore.hh>

@class BdnButtonClickManager;

namespace bdn::mac
{
    class ButtonCore : public ViewCore, virtual public bdn::Button::Core
    {
      private:
        static NSButton *_createNsButton();

      public:
        ButtonCore(const std::shared_ptr<bdn::ViewCoreFactory> &viewCoreFactory);
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
