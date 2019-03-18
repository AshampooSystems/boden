#pragma once

#include <bdn/java/FieldAccessor.h>
#include <bdn/java/ObjectFieldKind.h>
#include <bdn/java/Reference.h>
#include <bdn/java/StaticFieldKind.h>

#include <utility>

namespace bdn::java
{
    /** Represents the field of a Java-side object or class and enables
     * access to it.
     *
     *  The template parameter NativeType is the type of the field. It can
     * be a simple type like int or bool, a standard type like String, or a
     * class derived from JObject.
     *
     *  The template parameter Kind specifies what type of field this is. It
     * can be either ObjectFieldKind for an instance field or
     * StaticFieldKind for a static field.
     *
     *  Field objects are light-weight and can be returned and passed around
     * by value. The Field instance encapsulates a reference to the
     * Java-side object or class (depending on the Field's \c Kind
     * parameter), so the object/class is kept alive by the field instance
     * itself.
     *
     *  Field instances are self-contained, so it is NOT necessary to keep
     * an accessor object (subclass of JObject) for the Java-side object
     * alive, even if the Field instance was returned by the accessor object
     *
     * */
    template <class NativeType, class Kind> class Field
    {
      public:
        /** \param contextRef reference to the java entity that owns the
         * field. What kind of entity this is depends on the Kind template
         * parameter. For StaticFieldKind it is a reference to the class
         * object. For ObjectFieldKind it is a reference to the object /
         * instance. \param fieldId the id of the field.
         *  */
        Field(const Reference &contextRef, const typename Kind::template Id<NativeType> &fieldId)
            : _contextRef(contextRef), _id(fieldId)
        {}

        Field(const Field &o) = default;
        Field(Field &&o) noexcept = default;

        void set(const NativeType &newValue)
        {
            FieldAccessor<Kind, NativeType>::setValue((typename Kind::ContextJType)_contextRef.getJObject(),
                                                      _id.getId(), newValue);
        }

        NativeType get() const
        {
            return FieldAccessor<Kind, NativeType>::getValue((typename Kind::ContextJType)_contextRef.getJObject(),
                                                             _id.getId());
        }

        Field &operator=(const NativeType &newValue)
        {
            set(newValue);
            return *this;
        }

        operator NativeType() const { return get(); }

      private:
        Reference _contextRef;
        typename Kind::template Id<NativeType> _id;
    };
}
