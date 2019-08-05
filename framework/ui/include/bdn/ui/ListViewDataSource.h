#pragma once

#include <bdn/ui/View.h>
#include <string>

namespace bdn::ui
{
    class ListViewDataSource
    {
      public:
        virtual ~ListViewDataSource() = default;

        virtual size_t numberOfRows() = 0;
        virtual std::shared_ptr<View> viewForRowIndex(size_t rowIndex, std::shared_ptr<View> reusableView) = 0;
        virtual float heightForRowIndex(size_t rowIndex) = 0;
    };
}
