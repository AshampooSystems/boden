#pragma once

#include <bdn/ui/View.h>
#include <string>

namespace bdn::ui
{
    class ListView;

    class ListViewDataSource
    {
      public:
        virtual ~ListViewDataSource() = default;

        virtual size_t numberOfRows() = 0;

        virtual std::shared_ptr<View> viewForRowIndex(const std::shared_ptr<ListView> &listView, size_t rowIndex,
                                                      std::shared_ptr<View> reusableView) = 0;

        // viewForRowIndex is deprecated, override viewForRowIndex(ListView, size_t, View) instead
        [[deprecated("Use viewForRowIndex(ListView, size_t, View) instead")]] virtual std::shared_ptr<View>
        viewForRowIndex(size_t rowIndex, std::shared_ptr<View> reusableView) final
        {
            return nullptr;
        }

        virtual float heightForRowIndex(size_t rowIndex) = 0;
    };
}
