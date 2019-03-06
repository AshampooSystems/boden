#pragma once

#include <bdn/java/Field.h>
#include <bdn/java/ObjectFieldKind.h>

namespace bdn::java
{
    /** Represents an object field of a Java-side object and enables access
     * to it.
     *
     *  The template parameter NativeType is the type of the field. It can
     * be a simple type like int or bool, a standard type like String, or a
     * class derived from JObject.
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
    template <class NativeType> class ObjectField : public Field<NativeType, ObjectFieldKind>
    {
      public:
        typedef ObjectFieldKind::template Id<NativeType> Id;

        ObjectField(const Reference &obj, const ObjectFieldKind::Id<NativeType> &fieldId)
            : Field<NativeType, ObjectFieldKind>(obj, fieldId)
        {}
    };
}