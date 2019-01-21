#pragma once

#include <bdn/property/Backing.h>
#include <functional>

namespace bdn
{

    template <class ValType> class GetterSetterBacking : public Backing<ValType>
    {
      public:
        using GetterFunc = std::function<ValType()>;
        using SetterFunc = std::function<bool(const ValType &)>;

        template <typename GetterType, typename SetterType>
        GetterSetterBacking(GetterType getter, SetterType setter, ValType *member = nullptr)
            : _member(member), _getter(getter), _setter(setter)
        {
            // It is invalid to provide nullptr for both member and getter as
            // this would render the backing unreadable to the property. In this
            // case, raise an exception at runtime.
            if (_member == nullptr && _getter == nullptr) {
                throw std::invalid_argument("Either member or getter must be specified");
            }
        }

        // Not default constructible, not copy-constructible
        GetterSetterBacking() = delete;
        GetterSetterBacking(const GetterSetterBacking &) = delete;

        virtual ValType get() const override
        {
            if (_getter == nullptr) {
                // If no getter is specified, the default implementation returns
                // the member value as is.
                return *_member;
            }
            return _getter();
        }

        virtual void set(const ValType &value, bool notify = true) override
        {
            bool changed = false;

            if (_setter == nullptr && _member != nullptr) {
                // If no setter is specified, but a member is, substitute the
                // setter implementation with a default one, checking the value
                // and writing it if it is being changed.
                if (*_member != value) {
                    *_member = value;
                    changed = true;
                }
            } else if (_setter != nullptr) {
                // If a setter is specified, call it and note whether the value
                // has been changed.
                changed = _setter(value);
            } else {
                throw std::invalid_argument("Either member or setter must be specified");
            }

            if (changed && notify)
                this->_pOnChange->postNotification(this);
        }

      protected:
        ValType *_member;
        GetterFunc _getter;
        SetterFunc _setter;
    };
}