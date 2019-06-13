#pragma once

#include <memory>

namespace bdn
{
    template <typename T> struct has_init_method
    {
      private:
        typedef std::true_type yes;
        typedef std::false_type no;

        template <typename U> static auto test(int) -> decltype(std::declval<U>().init(), yes());
        template <typename> static no test(...);

      public:
        static constexpr bool value = std::is_same<decltype(test<T>(0)), yes>::value;
    };

    class NeedsInit
    {
    };

    constexpr static const NeedsInit needsInit;
}

namespace std
{
    template <typename T, typename... Arguments, typename std::enable_if<bdn::has_init_method<T>::value, int>::type = 0>
    std::shared_ptr<T> make_shared(bdn::NeedsInit needsInit, Arguments... arguments)
    {
        auto t = std::allocate_shared<T, std::allocator<T>>(std::allocator<T>(), needsInit, arguments...);
        t->init();
        return t;
    }
}
