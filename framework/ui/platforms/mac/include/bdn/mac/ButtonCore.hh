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
                NSButton *button = [[NSButton alloc] initWithFrame:NSMakeRect(0, 0, 0, 0)];

                [button setButtonType:NSButtonTypeMomentaryLight];
                [button setBezelStyle:NSBezelStyleRounded];

                return button;
            }

          public:
            ButtonCore(std::shared_ptr<Button> outerButton);

            void setLabel(const String &label) override { ButtonCoreBase::setLabel(label); }

            Rect adjustAndSetBounds(const Rect &requestedBounds) override
            {
                Rect adjustedBounds = ChildViewCore::adjustAndSetBounds(requestedBounds);

                _updateBezelStyle();

                return adjustedBounds;
            }

            void setPadding(const std::optional<UIMargin> &padding) override
            {
                ChildViewCore::setPadding(padding);

                // we may need to update the bezel style.
            }

            Size calcPreferredSize(const Size &availableSpace = Size::none()) const override
            {
                // the bezel style influences the fitting size. To get
                // consistent values here we have to ensure that we use the same
                // bezel style each time we calculate the size.

                NSBezelStyle bezelStyle = _nsButton.bezelStyle;
                if (bezelStyle != NSBezelStyleRounded)
                    _nsButton.bezelStyle = NSBezelStyleRounded;

                Size size = ButtonCoreBase::calcPreferredSize(availableSpace);

                if (bezelStyle != NSBezelStyleRounded)
                    _nsButton.bezelStyle = bezelStyle;

                return size;
            }

            void generateClick();

          protected:
            Margin getPaddingIncludedInFittingSize() const override
            {
                // the button automatically includes some padding in the fitting
                // size. This is rougly this:

                return uiMarginToDipMargin(UIMargin(UILength::sem(0.5), UILength::sem(1)));
            }

          private:
            void _updateBezelStyle();

            BdnButtonClickManager *_clickManager;

            NSBezelStyle _currBezelStyle;
            int _heightWithRoundedBezelStyle;
        };
    }
}
