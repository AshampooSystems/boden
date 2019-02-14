#pragma once

#include <bdn/ListViewDataSource.h>

namespace bdn
{
    class ListViewCore
    {
      public:
        virtual void reloadData() = 0;
    };
}
