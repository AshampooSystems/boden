#pragma once

#include <bdn/Checkbox.h>
#include <bdn/CheckboxCore.h>
#include <bdn/SwitchCore.h>

#import <bdn/mac/ViewCore.hh>
#import <bdn/mac/ui_util.hh>
#import <bdn/mac/util.hh>

namespace bdn::mac
{
    class CheckboxCore : public ViewCore, virtual public bdn::CheckboxCore
    {
      private:
        static NSButton *createNSButton();

      public:
        CheckboxCore(const std::shared_ptr<bdn::ViewCoreFactory> &viewCoreFactory);

      public:
        void handleClick();

      private:
        NSButton *_nsButton;
    };
}
