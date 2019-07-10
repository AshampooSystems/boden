#pragma once

#include <bdn/property/Backing.h>
#include <optional>

namespace bdn
{

    template <class ValType> class ValueBacking : public Backing<ValType>
    {
      public:
        ValueBacking() : _value() {}
        ValueBacking(ValType value) : _value(value) {}
        ValueBacking(const ValueBacking &other) : _value(other.get()) {}

        ValType get() const override { return _value; }

        void set(const ValType &value, bool notify = true) override
        {
            bool changed = false;

            {
                if (Compare<ValType>::notEqual(_value, value)) {
                    _value = value;
                    changed = true;
                }
            }

            if (changed && notify) {
                this->_onChange.notify(Backing<ValType>::shared_from_this());
            }
        }

        ValueBacking &operator=(const ValueBacking &other)
        {
            // Preserve own value memory and set value from other backing
            this->set(other.get());
            return *this;
        }

      public:
        template <class T> struct Compare
        {
            static const bool isComparable = true;
            static bool notEqual(const T &left, const T &right) { return left != right; }
        };

        template <class _fp> struct Compare<std::function<_fp>>
        {
            static const bool isComparable = false;
            static bool notEqual(const std::function<_fp> &left, const std::function<_fp> &right) { return true; }
        };

      protected:
        ValType _value;
    };
}
