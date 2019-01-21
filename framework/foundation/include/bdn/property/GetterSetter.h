#pragma once

#include <functional>

namespace bdn
{

    template <class ValType> class GetterSetter
    {
      public:
        using GetterFunc = std::function<ValType()>;
        using SetterFunc = std::function<bool(const ValType &)>;

        template <typename OwnerType, typename GetterType, typename SetterType>
        GetterSetter(OwnerType owner, GetterType getter, SetterType setter, ValType *member = nullptr)
            : _member(member), _getter(bindOptionalGetter(owner, getter)), _setter(bindOptionalSetter(owner, setter))
        {}

        ValType *member() const { return _member; }

        GetterFunc getter() const { return _getter; }

        SetterFunc setter() const { return _setter; }

      private:
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

        ValType *_member;
        GetterFunc _getter;
        SetterFunc _setter;
    };
}