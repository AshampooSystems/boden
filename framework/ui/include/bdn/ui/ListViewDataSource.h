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

        virtual std::shared_ptr<View> viewForRowIndex(const std::shared_ptr<ListView> &listView, size_t rowIndex,
                                                      std::shared_ptr<View> reusableView) = 0;

        virtual size_t numberOfRows(const std::shared_ptr<ListView> &listView) = 0;
        virtual float heightForRowIndex(const std::shared_ptr<ListView> &listView, size_t rowIndex) = 0;
        virtual bool shouldSelectRow(const std::shared_ptr<ListView> &listView, size_t rowIndex) { return true; }

      public:
        [[deprecated("Use viewForRowIndex(ListView, size_t, View) instead")]] virtual std::shared_ptr<View>
        viewForRowIndex(size_t rowIndex, std::shared_ptr<View> reusableView) final
        {
            return nullptr;
        }

        [[deprecated("Use numberOfRows(ListView) instead")]] virtual size_t numberOfRows() final { return 0; };
        [[deprecated("Use heightForRowIndex(ListView, size_t) instead")]] virtual float
        heightForRowIndex(size_t rowIndex) final
        {
            return 0;
        };
    };
}
