#pragma once

#include <bdn/View.h>
#include <bdn/ListViewDataSource.h>

namespace bdn
{
    class ListView : public View
    {
      public:
        static constexpr char coreTypeName[] = "bdn.ListViewCore";

      public:
        ListView();
        virtual ~ListView() = default;

        void reloadData();

        String getCoreTypeName() const override;

        Property<std::shared_ptr<ListViewDataSource>> dataSource;
        Property<std::optional<size_t>> selectedRowIndex;
    };
}
