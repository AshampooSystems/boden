#pragma once

#include <bdn/TextField.h>
#include <bdn/ios/ViewCore.hh>
#include <bdn/ios/util.hh>

@class BdnTextFieldDelegate;

namespace bdn
{
    namespace ios
    {

        class TextFieldCore : public ViewCore, virtual public ITextFieldCore
        {
          public:
            TextFieldCore(std::shared_ptr<TextField> outerTextField);
            ~TextFieldCore();

            void setText(const String &text) override;

          private:
            BdnTextFieldDelegate *_delegate;
        };
    }
}
