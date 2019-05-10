#pragma once

#include <bdn/mac/ViewCore.hh>
#include <bdn/mac/util.hh>
#include <bdn/ui/TextField.h>

@class BdnTextFieldDelegate;

namespace bdn::ui::mac
{
    class TextFieldCore : public ViewCore, virtual public TextField::Core
    {
      private:
        static NSTextField *_createNsTextView();

      public:
        TextFieldCore(const std::shared_ptr<ViewCoreFactory> &viewCoreFactory);
        ~TextFieldCore();

        void init() override;

      private:
        BdnTextFieldDelegate *_delegate;
    };
}
