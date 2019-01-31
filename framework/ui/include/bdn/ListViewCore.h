#pragma once

#include <bdn/ListViewDataSource.h>

namespace bdn
{
    class ListViewCore
    {
      public:
        virtual void setDataSource(const std::shared_ptr<ListViewDataSource> &dataSource) = 0;
        virtual void reloadData() = 0;
    };
}
