#pragma once

#include <bdn/GlobalStack.h>
#include <bdn/String.h>

namespace bdn
{
    class AttributedString
    {
      public:
        static std::function<std::shared_ptr<AttributedString>()> defaultCreateAttributedString();

      public:
        using CreatorStack =
            GlobalStack<std::function<std::shared_ptr<AttributedString>()>, &defaultCreateAttributedString>;

      public:
        virtual ~AttributedString() = default;

      public:
        virtual bool fromHtml(const String &html) = 0;
        virtual String toHtml() const = 0;
    };
}

namespace std
{
    template <> inline std::shared_ptr<bdn::AttributedString> make_shared<bdn::AttributedString>()
    {
        return bdn::AttributedString::CreatorStack::top()();
    }
}
