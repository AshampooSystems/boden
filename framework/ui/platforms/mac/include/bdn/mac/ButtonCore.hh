#pragma once

#include <bdn/Button.h>
#include <bdn/IButtonCore.h>

#import <bdn/mac/ButtonCoreBase.hh>

@class BdnButtonClickManager;

namespace bdn::mac
{
    class ButtonCore : public ButtonCoreBase, virtual public IButtonCore
    {
      private:
        static NSButton *_createNsButton(std::shared_ptr<Button> outerButton);

      public:
        ButtonCore(std::shared_ptr<Button> outerButton);

        void setLabel(const String &label) override;

        Size sizeForSpace(Size availableSpace) const override;

        void generateClick();

      private:
        void _updateBezelStyle();

        BdnButtonClickManager *_clickManager;

        NSBezelStyle _currBezelStyle;
        int _heightWithRoundedBezelStyle;
    };
}
