#pragma once

#include <bdn/ListViewDataSource.h>

namespace bdn
{
    class ListViewCore
    {
      public:
        Property<std::shared_ptr<ListViewDataSource>> dataSource;
        Property<std::optional<size_t>> selectedRowIndex;

      public:
        virtual void reloadData() = 0;
    };
}
