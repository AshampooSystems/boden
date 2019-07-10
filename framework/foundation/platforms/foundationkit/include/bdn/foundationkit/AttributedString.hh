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
        bool fromHtml(const String &str);

      public:
        NSAttributedString *nsAttributedString() { return _nsAttributedString; }

      private:
        NSAttributedString *_nsAttributedString = nullptr;
    };
}
