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
                _field = std::make_unique<StaticFieldImpl<NativeType>>(
                    Class::getStaticClass_(), StaticFieldImpl<NativeType>::Id(Class::getStaticClass_(), _kFieldName));
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
                typename StaticFieldImpl<NativeType>::Id id(Class::getStaticClass_(), _kFieldName);
                StaticFieldImpl<NativeType> field(Class::getStaticClass_(), id);
                _value = (NativeType)field;
            }
            return *_value;
        }

        mutable std::optional<NativeType> _value;
        const char *_kFieldName;
    };

    template <class NativeType> class JavaEnum
    {
      public:
        constexpr JavaEnum(const char *kFieldName) : _kFieldName(kFieldName) {}

        operator NativeType() const
        {
            if (!_value) {
                typename StaticFieldImpl<NativeType>::Id id(NativeType::getStaticClass_(), _kFieldName);
                StaticFieldImpl<NativeType> field(NativeType::getStaticClass_(), id);
                _value = field.get();
            }
            return *_value;
        }

        mutable std::optional<NativeType> _value;
        const char *_kFieldName;
    };
}
