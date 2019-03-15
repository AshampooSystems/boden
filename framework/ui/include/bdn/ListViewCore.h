#pragma once

#include <bdn/ListViewDataSource.h>

namespace bdn
{
    class ListViewCore
    {
        friend class ListView;

      public:
        Property<bool> enableRefresh;
        Property<std::shared_ptr<ListViewDataSource>> dataSource;
        Property<std::optional<size_t>> selectedRowIndex;

      public:
        virtual void reloadData() = 0;
        virtual void refreshDone() = 0;

      protected:
        WeakCallback<void()> _refreshCallback;
    };
}
