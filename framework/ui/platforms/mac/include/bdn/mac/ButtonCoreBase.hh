#pragma once

#include <bdn/ClickEvent.h>

#import <bdn/mac/ChildViewCore.hh>

#import <bdn/mac/util.hh>

namespace bdn::mac
{
    class ButtonCoreBase : public ChildViewCore
    {
      public:
        ButtonCoreBase(std::shared_ptr<View> outer, NSButton *nsButton);

      public:
        void setLabel(const String &label);

      protected:
        double getFontSize() const override;

        NSButton *_nsButton;
    };
}
