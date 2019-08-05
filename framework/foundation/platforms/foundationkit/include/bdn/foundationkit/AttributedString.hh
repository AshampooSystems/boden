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

        void addAttribute(std::string attributeName, std::any value, Range range) override;
        void addAttributes(AttributeMap attributes, Range range) override;

      public:
        void fromString(const std::string &text) override;

      public:
        bool fromHTML(const std::string &str) override;
        std::string toHTML() const override;

      public:
        NSAttributedString *nsAttributedString() { return _nsAttributedString; }

      private:
        NSDictionary *attributeMapToDict(const AttributeMap &attributeMap);

      private:
        NSMutableAttributedString *_nsAttributedString = nullptr;
    };
}
