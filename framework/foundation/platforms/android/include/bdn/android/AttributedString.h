#pragma once

#include <bdn/AttributedString.h>
#include <bdn/android/wrapper/SpannableStringBuilder.h>

namespace bdn::android
{
    class AttributedString : public bdn::AttributedString
    {
      public:
        AttributedString();

      public:
        void addAttribute(std::string attributeName, std::any value, Range range) override;
        void addAttributes(AttributeMap attributes, Range range) override;

      public:
        void fromString(const std::string &text) override;

      public:
        bool fromHTML(const std::string &html) override;
        std::string toHTML() const override;

      public:
        wrapper::Spanned spanned() { return _spannedBuilder; }

      public:
        Notifier<std::string> &linkClicked() { return _linkClicked; }

      private:
        void visitWithObjectFromAttribute(AttributeMap::const_reference attribute,
                                          std::function<void(java::wrapper::Object)> func);

      private:
        wrapper::SpannableStringBuilder _spannedBuilder;
        Notifier<std::string> _linkClicked;
    };
}
