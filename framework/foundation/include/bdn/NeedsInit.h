#pragma once

#include <memory>

namespace bdn
{
    template <class T> constexpr auto has_init_method(T *x) -> decltype(x->init(), std::true_type{}) { return {}; }
    constexpr auto has_init_method(...) -> std::false_type { return {}; }

    class NeedsInit
    {
    };

    constexpr static const NeedsInit needsInit;
}

namespace std
{
    template <typename T, typename... Arguments>
    std::shared_ptr<T> make_shared(bdn::NeedsInit needsInit, Arguments... arguments)
    {
        auto t = std::allocate_shared<T, std::allocator<T>>(std::allocator<T>(), needsInit, arguments...);

        if constexpr (bdn::has_init_method((T *)nullptr)) {
            t->init();
        }

        return t;
    }
}
