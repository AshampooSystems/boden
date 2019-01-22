#pragma once

#include <bdn/ClickEvent.h>

#import <bdn/mac/ChildViewCore.hh>

#import <bdn/mac/util.hh>

namespace bdn
{
    namespace mac
    {

        class ButtonCoreBase : public ChildViewCore
        {
          public:
            ButtonCoreBase(std::shared_ptr<View> outer, NSButton *nsButton) : ChildViewCore(outer, nsButton)
            {
                _nsButton = nsButton;
            }

            void setLabel(const String &label)
            {
                NSString *macLabel = stringToNSString(label);
                [_nsButton setTitle:macLabel];
            }

          protected:
            double getFontSize() const override { return _nsButton.font.pointSize; }

            NSButton *_nsButton;
        };
    }
}
