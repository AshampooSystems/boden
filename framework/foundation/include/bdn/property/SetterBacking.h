#pragma once

#include <bdn/property/Backing.h>

namespace bdn
{

    template <class ValType> class SetterBacking : public Backing<ValType>
    {
        using value_accessor_t = typename Backing<ValType>::value_accessor_t;

      public:
        using SetterFunc = std::function<bool(ValType &, ValType)>;

        SetterBacking() = delete;

        SetterBacking(const SetterBacking &other) : _value(other._value), _setter(other._setter) {}
        SetterBacking(ValType value) : _value(value) {}
        SetterBacking(SetterFunc setter) : _setter(setter) {}

        ValType get() const override { return _value; }

        void set(const ValType &value, bool notify = true) override
        {
            if (_setter == nullptr) {
                _value = value;
            } else if (_setter(_value, value) && notify) {
                this->_pOnChange->notify(
                    std::dynamic_pointer_cast<value_accessor_t>(Backing<ValType>::shared_from_this()));
            }
        }

      protected:
        ValType _value;
        SetterFunc _setter;
    };
}
