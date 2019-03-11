#pragma once

#include <optional>

namespace bdn
{
    template <class T> class OfferedValue
    {
      public:
        T set(T v)
        {
            T old = get();
            _own = v;
            return old;
        }

        T setOffered(T v)
        {
            T old = get();
            _offered = std::move(v);
            return old;
        }

        T get()
        {
            if (_own) {
                return *_own;
            }
            return _offered;
        }

      private:
        T _offered;
        std::optional<T> _own;
    };
}
