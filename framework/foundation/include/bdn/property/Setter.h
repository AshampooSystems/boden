#pragma once

#include <functional>

namespace bdn
{

    template <class ValType> class Setter
    {
      public:
        using SetterFunc = std::function<bool(ValType &, ValType)>;

        template <typename SetterType> Setter(SetterType setter_) : _setter(setter_) {}

        SetterFunc setter() const { return _setter; }

      private:
        SetterFunc _setter = nullptr;
    };
}