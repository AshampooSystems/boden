#pragma once

#include <bdn/ios/ViewCore.hh>
#include <bdn/ios/util.hh>
#include <bdn/ui/TextField.h>

@class BdnTextFieldDelegate;

namespace bdn::ui::ios
{
    class TextFieldCore : public ViewCore, virtual public TextField::Core
    {
      public:
        TextFieldCore(const std::shared_ptr<ViewCoreFactory> &viewCoreFactory);
        ~TextFieldCore();

      public:
        void init() override;

      public:
        void focus() override;

      public:
        float calculateBaseline(Size forSize) const override;

      private:
        void setFont(const Font &font);
        void setAutocorrectionType(const AutocorrectionType autocorrectionType);
        void setReturnKeyType(const ReturnKeyType returnKeyType);
        void setPlaceholder(const Text &text);
        void setTextInputType(const TextInputType &textInputType);

      private:
        BdnTextFieldDelegate *_delegate;
    };
}
