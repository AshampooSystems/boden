#pragma once

#include <bdn/AttributedString.h>
#include <bdn/android/wrapper/Spanned.h>

namespace bdn::android
{
    class AttributedString : public bdn::AttributedString
    {
      public:
        AttributedString();

      public:
        bool fromHtml(const String &html) override;

      public:
        wrapper::Spanned spanned() { return _spanned; }

      private:
        wrapper::Spanned _spanned;
    };
}
