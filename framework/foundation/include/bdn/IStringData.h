#pragma once

#include <bdn/IBase.h>
#include <bdn/CharIterator.h>

namespace bdn
{

    class IStringData : virtual public IBase
    {
      public:
        virtual CharIterator begin() = 0;
        virtual CharIterator end() = 0;

        virtual bool operator==(CharIterator otherBegin, CharIterator otherEnd) = 0;
    };
}
