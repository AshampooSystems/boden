#pragma once

#include <bdn/TextField.h>
#include <bdn/mac/ViewCore.hh>
#include <bdn/mac/util.hh>

@class BdnTextFieldDelegate;

namespace bdn::mac
{
    class TextFieldCore : public ViewCore, virtual public bdn::TextField::Core
    {
      private:
        static NSTextField *_createNsTextView();

      public:
        TextFieldCore(const std::shared_ptr<bdn::ViewCoreFactory> &viewCoreFactory);
        ~TextFieldCore();

        void init() override;

      private:
        BdnTextFieldDelegate *_delegate;
    };
}
