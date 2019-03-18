#pragma once

#include <bdn/TextField.h>
#include <bdn/ios/ViewCore.hh>
#include <bdn/ios/util.hh>

@class BdnTextFieldDelegate;

namespace bdn::ios
{
    class TextFieldCore : public ViewCore, virtual public bdn::TextFieldCore
    {
      public:
        TextFieldCore(const std::shared_ptr<bdn::UIProvider> &uiProvider);
        ~TextFieldCore() override;

        void init() override;

      private:
        BdnTextFieldDelegate *_delegate;
    };
}
