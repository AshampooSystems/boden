#pragma once

#include <bdn/NeedsInit.h>
#include <bdn/ui/ViewCoreFactory.h>
#include <typeinfo>

namespace bdn::ui
{
    template <class BASE> class CoreLess : public BASE
    {
      public:
        explicit CoreLess(bdn::NeedsInit, std::shared_ptr<ViewCoreFactory> viewCoreFactory = nullptr)
            : BASE(viewCoreFactory)
        {}

        virtual void init() = 0;

      public:
        const std::type_info &typeInfoForCoreCreation() const { return typeid(BASE); }
    };
}
