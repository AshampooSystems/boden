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
            static StaticFieldImpl<int>::Id id(Class::getStaticClass_(), _kFieldName);
            static StaticFieldImpl<int> field(Class::getStaticClass_(), id);
            return (NativeType)field;
        }

        const char *_kFieldName;
    };

    template <class NativeType, class Class> class StaticFinalField
    {
      public:
        constexpr StaticFinalField(const char *kFieldName) : _kFieldName(kFieldName) {}

        operator NativeType() const
        {
            static StaticFieldImpl<int>::Id id(Class::getStaticClass_(), _kFieldName);
            static StaticFieldImpl<int> field(Class::getStaticClass_(), id);
            static NativeType value = (NativeType)field;
            return (NativeType)value;
        }

        const char *_kFieldName;
    };
}
