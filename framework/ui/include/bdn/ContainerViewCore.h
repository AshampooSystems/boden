#pragma once

#include <bdn/ViewCore.h>

namespace bdn
{
    class ContainerViewCore
    {
      public:
        virtual ~ContainerViewCore() = default;

      public:
        virtual void addChildView(std::shared_ptr<View> child) = 0;
        virtual void removeChildView(std::shared_ptr<View> child) = 0;

        virtual std::list<std::shared_ptr<View>> childViews() = 0;
    };
}
