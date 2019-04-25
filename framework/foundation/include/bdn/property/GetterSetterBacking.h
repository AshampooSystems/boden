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

        // Not default constructible
        GetterSetterBacking() = delete;

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

        template <typename OwnerType, typename GetterType, typename SetterType>
        GetterSetterBacking(OwnerType owner, GetterType getter, SetterType setter, ValType *member = nullptr)
            : _member(member), _getter(bindOptionalGetter(owner, getter)), _setter(bindOptionalSetter(owner, setter))
        {}

        GetterSetterBacking(const GetterSetterBacking &other)
            : _member(other._member), _getter(other._getter), _setter(other._setter)
        {}

        ValType get() const override
        {
            if (_getter == nullptr) {
                // If no getter is specified, the default implementation returns
                // the member value as is.
                return *_member;
            }
            return _getter();
        }

        void set(const ValType &value, bool notify = true) override
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

            if (changed && notify) {
                this->_pOnChange->notify(Backing<ValType>::shared_from_this());
            }
        }

      protected:
        template <typename OwnerType, typename GetterType>
        static GetterFunc bindOptionalGetter(OwnerType owner_, GetterType getter_)
        {
            return std::bind(getter_, owner_);
        }

        template <typename OwnerType> static GetterFunc bindOptionalGetter(OwnerType owner_, std::nullptr_t getter_)
        {
            return nullptr;
        }

        template <typename OwnerType, typename SetterType>
        static SetterFunc bindOptionalSetter(OwnerType owner_, SetterType setter_)
        {
            return std::bind(setter_, owner_, std::placeholders::_1);
        }

        template <typename OwnerType> static SetterFunc bindOptionalSetter(OwnerType owner_, std::nullptr_t setter_)
        {
            return nullptr;
        }

      protected:
        ValType *_member;
        GetterFunc _getter;
        SetterFunc _setter;
    };
}
