#pragma once

#include <any>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include <bdn/GlobalStack.h>
#include <bdn/Json.h>
#include <bdn/Notifier.h>
#include <bdn/String.h>

namespace bdn
{
    class AttributedString : public std::enable_shared_from_this<AttributedString>
    {
      public:
        struct Range
        {
            size_t start;
            size_t length;
        };

        using AttributeMap = std::map<String, std::any>;

      public:
        static std::function<std::shared_ptr<AttributedString>()> defaultCreateAttributedString();

      public:
        using CreatorStack =
            GlobalStack<std::function<std::shared_ptr<AttributedString>()>, &defaultCreateAttributedString>;

      public:
        virtual ~AttributedString() = default;

      public:
        virtual void addAttribute(String attributeName, std::any value, Range range) = 0;
        virtual void addAttributes(AttributeMap attributes, Range range) = 0;

      public:
        virtual void fromString(const String &text) = 0;

      public:
        virtual bool fromHTML(const String &html) = 0;
        virtual String toHTML() const = 0;

      public:
        virtual void fromJSON(const bdn::json &json);
        virtual bdn::json toJSON() const;
    };
}

namespace std
{
    template <> inline std::shared_ptr<bdn::AttributedString> make_shared<bdn::AttributedString>()
    {
        return bdn::AttributedString::CreatorStack::top()();
    }
}
