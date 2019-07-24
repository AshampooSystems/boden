#pragma once

#include <bdn/java/Field.h>
#include <bdn/java/StaticFieldKind.h>
#include <bdn/java/wrapper/Class.h>

namespace bdn::java
{
    template <class NativeType> class StaticFieldImpl : public Field<NativeType, StaticFieldKind>
    {
      public:
        using Id = StaticFieldKind::template Id<NativeType>;

        StaticFieldImpl(wrapper::Class &cls, const Id &fieldId)
            : Field<NativeType, StaticFieldKind>(cls.getRef_(), fieldId)
        {}
    };

    template <class NativeType, class Class> class StaticField
    {
      public:
        constexpr StaticField(const char *kFieldName) : _kFieldName(kFieldName) {}

        operator NativeType() const
        {
            if (!_field) {
                _field = std::make_unique<StaticFieldImpl<int>>(
                    Class::getStaticClass_(), StaticFieldImpl<int>::Id(Class::getStaticClass_(), _kFieldName));
            }
            return (NativeType)*_field;
        }

        mutable std::unique_ptr<StaticFieldImpl<int>> _field;

        const char *_kFieldName;
    };

    template <class NativeType, class Class> class StaticFinalField
    {
      public:
        constexpr StaticFinalField(const char *kFieldName) : _kFieldName(kFieldName) {}

        operator NativeType() const
        {
            if (!_value) {
                StaticFieldImpl<int>::Id id(Class::getStaticClass_(), _kFieldName);
                StaticFieldImpl<int> field(Class::getStaticClass_(), id);
                _value = (NativeType)field;
            }
            return *_value;
        }

        mutable std::optional<NativeType> _value;
        const char *_kFieldName;
    };
}
