#pragma once

#include <bdn/String.h>
#include <bdn/View.h>

namespace bdn
{
    class ListViewDataSource
    {
      public:
        virtual ~ListViewDataSource() = default;

        virtual size_t numberOfRows() = 0;
        virtual String labelTextForRowIndex(size_t rowIndex) = 0;
        virtual std::shared_ptr<View> viewForRowIndex(size_t rowIndex, std::shared_ptr<View> reusableView) = 0;
    };
}
