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
        bool fromHTML(const String &str) override;
        String toHTML() const override;

      public:
        NSAttributedString *nsAttributedString() { return _nsAttributedString; }

      private:
        NSMutableAttributedString *_nsAttributedString = nullptr;
    };
}
