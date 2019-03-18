#pragma once

#include <bdn/property/Backing.h>
#include <bdn/property/Setter.h>

namespace bdn
{

    template <class ValType> class SetterBacking : public Backing<ValType>
    {
        using value_accessor_t = typename Backing<ValType>::value_accessor_t;

      public:
        using SetterFunc = typename Setter<ValType>::SetterFunc;

        // Not default constructible, not copy-constructible
        SetterBacking() = delete;
        SetterBacking(const SetterBacking &) = delete;

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