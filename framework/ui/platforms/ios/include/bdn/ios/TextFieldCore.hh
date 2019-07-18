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
        float calculateBaseline(Size forSize) const override;

      private:
        void setFont(const Font &font);
        void setAutocorrectionType(const AutocorrectionType autocorrectionType);

      private:
        BdnTextFieldDelegate *_delegate;
    };
}
