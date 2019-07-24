#pragma once

#include <bdn/AttributedString.h>

#import <Foundation/Foundation.h>

namespace bdn::fk
{
    class AttributedString : public bdn::AttributedString
    {
      public:
        AttributedString();

      public:
        void setAttributes(AttributeMap attributes, Range range);

        void addAttribute(String attributeName, std::any value, Range range) override;
        void addAttributes(AttributeMap attributes, Range range) override;

      public:
        void fromString(const String &text) override;

      public:
        bool fromHTML(const String &str) override;
        String toHTML() const override;

      public:
        NSAttributedString *nsAttributedString() { return _nsAttributedString; }

      private:
        NSDictionary *attributeMapToDict(const AttributeMap &attributeMap);

      private:
        NSMutableAttributedString *_nsAttributedString = nullptr;
    };
}
