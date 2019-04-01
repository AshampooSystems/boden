#pragma once

#include <bdn/TextField.h>
#include <bdn/ios/ViewCore.hh>
#include <bdn/ios/util.hh>

@class BdnTextFieldDelegate;

namespace bdn::ios
{
    class TextFieldCore : public ViewCore, virtual public bdn::TextField::Core
    {
      public:
        TextFieldCore(const std::shared_ptr<bdn::ViewCoreFactory> &viewCoreFactory);
        ~TextFieldCore();

      public:
        void init() override;

      private:
        BdnTextFieldDelegate *_delegate;
    };
}
