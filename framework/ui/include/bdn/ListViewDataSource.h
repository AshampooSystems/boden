#pragma once

#include <bdn/String.h>

namespace bdn
{
    class ListViewDataSource
    {
      public:
        virtual ~ListViewDataSource() = default;

        virtual size_t numberOfRows() = 0;
        virtual String labelTextForRowIndex(size_t rowIndex) = 0;
    };
}
