#pragma once

#include <typeinfo>

namespace bdn::ui
{
    template <class BASE> class CoreLess : public BASE
    {
      public:
        using BASE::BASE;

        const std::type_info &typeInfoForCoreCreation() const { return typeid(BASE); }
    };
}
