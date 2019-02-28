#pragma once

#include <bdn/Button.h>
#include <bdn/IButtonCore.h>

#import <bdn/mac/ButtonCoreBase.hh>

@class BdnButtonClickManager;

namespace bdn
{
    namespace mac
    {

        class ButtonCore : public ButtonCoreBase, virtual public IButtonCore
        {
          private:
            static NSButton *_createNsButton(std::shared_ptr<Button> outerButton)
            {
                NSButton *button = [[NSButton alloc] initWithFrame:rectToMacRect(outerButton->geometry, -1)];

                [button setButtonType:NSButtonTypeMomentaryLight];
                [button setBezelStyle:NSBezelStyleRounded];

                return button;
            }

          public:
            ButtonCore(std::shared_ptr<Button> outerButton);

            void setLabel(const String &label) override { ButtonCoreBase::setLabel(label); }

            Size sizeForSpace(Size availableSpace) const override
            {
                // the bezel style influences the fitting size. To get
                // consistent values here we have to ensure that we use the same
                // bezel style each time we calculate the size.

                NSBezelStyle bezelStyle = _nsButton.bezelStyle;
                if (bezelStyle != NSBezelStyleRounded)
                    _nsButton.bezelStyle = NSBezelStyleRounded;

                Size size = ButtonCoreBase::sizeForSpace(availableSpace);

                if (bezelStyle != NSBezelStyleRounded)
                    _nsButton.bezelStyle = bezelStyle;

                return size;
            }

            void generateClick();

          private:
            void _updateBezelStyle();

            BdnButtonClickManager *_clickManager;

            NSBezelStyle _currBezelStyle;
            int _heightWithRoundedBezelStyle;
        };
    }
}
