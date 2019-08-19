#pragma once

#include <optional>

namespace bdn
{
    template <class T> class ValueWithFallback
    {
      public:
        T set(T v)
        {
            T old = get();
            _own = v;
            return old;
        }

        T setFallback(T v)
        {
            T old = get();
            _fallback = std::move(v);
            return old;
        }

        T get()
        {
            if (_own) {
                return *_own;
            }
            return _fallback;
        }

        void unset() { _own = std::nullopt; }

      private:
        T _fallback;
        std::optional<T> _own;
    };
}
