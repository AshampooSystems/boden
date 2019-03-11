#pragma once

#include <bdn/TextField.h>
#include <bdn/TextFieldCore.h>
#include <bdn/mac/ViewCore.hh>
#include <bdn/mac/util.hh>

@class BdnTextFieldDelegate;

namespace bdn::mac
{
    class TextFieldCore : public ViewCore, virtual public bdn::TextFieldCore
    {
      private:
        static NSTextField *_createNsTextView();

      public:
        TextFieldCore();
        virtual ~TextFieldCore();

        virtual void init() override;

      private:
        BdnTextFieldDelegate *_delegate;
    };
}
