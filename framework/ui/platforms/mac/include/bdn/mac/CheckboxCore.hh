#pragma once

#include <bdn/ui/Checkbox.h>

#import <bdn/mac/UIUtil.hh>
#import <bdn/mac/ViewCore.hh>
#import <bdn/mac/util.hh>

@class BdnCheckboxClickManager;

namespace bdn::ui::mac
{
    class CheckboxCore : public ViewCore, virtual public Checkbox::Core
    {
      private:
        static NSButton *createNSButton();

      public:
        CheckboxCore(const std::shared_ptr<ViewCoreFactory> &viewCoreFactory);
        void init() override;

      public:
        void handleClick();

      private:
        NSButton *_nsButton;
        BdnCheckboxClickManager *_clickManager;
    };
}
