#ifndef BDN_JAVA_FieldAccessor_H_
#define BDN_JAVA_FieldAccessor_H_


#include <bdn/java/TypeConversion.h>

namespace bdn
{
namespace java
{


/** Allows access to fields of java-side objects.
 *
 *  Usage:
 *
 *  \code
 *
 *  jobject  context = ...; *
 *  jfieldID fieldID = ...;
 *  int      newValue = ...;
 *
 *  // for object fields *
 *  int oldVal = FieldAccessor<int, ObjectFieldContext>::getValue(context, fieldId); *
 *  FieldAccessor<int, ObjectFieldContext>::setValue(context, fieldId, newValue);
 *
 *  // for static fields
 *  jclass  cls = ...;
 *
 *  int oldVal = FieldAccessor<int, StaticFieldContext>::getValue(cls, fieldId); *
 *  FieldAccessor<int, StaticFieldContext>::setValue(cls, fieldId, newValue);
 *
 *  \endcode
 **/
template<class FieldKind, typename NATIVE_TYPE>
class FieldAccessor
{
public:
    typedef NATIVE_TYPE NativeType;

    static NativeType getValue(typename FieldKind::ContextJType context, jfieldID fieldId)
    {
        return TypeConversion<NativeType>::takeOwnershipOfJavaValueAndConvertToNative( FieldKind::getObjectField(context, fieldId) );
    }

    static void setValue(typename FieldKind::ContextJType context, jfieldID fieldId, const NativeType& value)
    {
        std::list<Reference> createdJavaObjects;
        return FieldKind::setObjectField( context, fieldId, TypeConversion<NativeType>::nativeToJava(value, createdJavaObjects) );
    }
};



template<class FieldKind>
class FieldAccessor<FieldKind, bool>
{
public:
    typedef bool NativeType;

    static NativeType getValue(typename FieldKind::ContextJType context, jfieldID fieldId)
    {
        return TypeConversion<NativeType>::takeOwnershipOfJavaValueAndConvertToNative( FieldKind::getBooleanField(context, fieldId) );
    }

    static void setValue(typename FieldKind::ContextJType context, jfieldID fieldId, NativeType value)
    {
        std::list<Reference> createdJavaObjects;
        return FieldKind::setBooleanField( context, fieldId, TypeConversion<NativeType>::nativeToJava(value, createdJavaObjects) );
    }
};


template<class FieldKind>
class FieldAccessor<FieldKind, int>
{
public:
    typedef int NativeType;

    static NativeType getValue(typename FieldKind::ContextJType context, jfieldID fieldId)
    {
        return TypeConversion<NativeType>::takeOwnershipOfJavaValueAndConvertToNative( FieldKind::getIntField(context, fieldId) );
    }

    static void setValue(typename FieldKind::ContextJType context, jfieldID fieldId, const NativeType& value)
    {
        std::list<Reference> createdJavaObjects;
        return FieldKind::setIntField( context, fieldId, TypeConversion<NativeType>::nativeToJava(value, createdJavaObjects) );
    }
};


template<class FieldKind>
class FieldAccessor<FieldKind, short>
{
public:
    typedef short NativeType;

    static NativeType getValue(typename FieldKind::ContextJType context, jfieldID fieldId)
    {
        return TypeConversion<NativeType>::takeOwnershipOfJavaValueAndConvertToNative( FieldKind::getShortField(context, fieldId) );
    }

    static void setValue(typename FieldKind::ContextJType context, jfieldID fieldId, const NativeType& value)
    {
        std::list<Reference> createdJavaObjects;
        return FieldKind::setShortField( context, fieldId, TypeConversion<NativeType>::nativeToJava(value, createdJavaObjects) );
    }
};


template<class FieldKind>
class FieldAccessor<FieldKind, int64_t>
{
public:
    typedef int64_t NativeType;

    static NativeType getValue(typename FieldKind::ContextJType context, jfieldID fieldId)
    {
        return TypeConversion<NativeType>::takeOwnershipOfJavaValueAndConvertToNative( FieldKind::getLongField(context, fieldId) );
    }

    static void setValue(typename FieldKind::ContextJType context, jfieldID fieldId, const NativeType& value)
    {
        std::list<Reference> createdJavaObjects;
        return FieldKind::setLongField( context, fieldId, TypeConversion<NativeType>::nativeToJava(value, createdJavaObjects) );
    }
};


template<class FieldKind>
class FieldAccessor<FieldKind, char32_t>
{
public:
    typedef char32_t NativeType;

    static NativeType getValue(typename FieldKind::ContextJType context, jfieldID fieldId)
    {
        return TypeConversion<NativeType>::takeOwnershipOfJavaValueAndConvertToNative( FieldKind::getCharField(context, fieldId) );
    }

    static void setValue(typename FieldKind::ContextJType context, jfieldID fieldId, const NativeType& value)
    {
        std::list<Reference> createdJavaObjects;
        return FieldKind::setCharField( context, fieldId, TypeConversion<NativeType>::nativeToJava(value, createdJavaObjects) );
    }
};


template<class FieldKind>
class FieldAccessor<FieldKind, int8_t>
{
public:
    typedef int8_t NativeType;

    static NativeType getValue(typename FieldKind::ContextJType context, jfieldID fieldId)
    {
        return TypeConversion<NativeType>::takeOwnershipOfJavaValueAndConvertToNative( FieldKind::getByteField(context, fieldId) );
    }

    static void setValue(typename FieldKind::ContextJType context, jfieldID fieldId, const NativeType& value)
    {
        std::list<Reference> createdJavaObjects;
        return FieldKind::setByteField( context, fieldId, TypeConversion<NativeType>::nativeToJava(value, createdJavaObjects) );
    }
};



template<class FieldKind>
class FieldAccessor<FieldKind, float>
{
public:
    typedef float NativeType;

    static NativeType getValue(typename FieldKind::ContextJType context, jfieldID fieldId)
    {
        return TypeConversion<NativeType>::takeOwnershipOfJavaValueAndConvertToNative( FieldKind::getFloatField(context, fieldId) );
    }

    static void setValue(typename FieldKind::ContextJType context, jfieldID fieldId, const NativeType& value)
    {
        std::list<Reference> createdJavaObjects;
        return FieldKind::setFloatField( context, fieldId, TypeConversion<NativeType>::nativeToJava(value, createdJavaObjects) );
    }
};


template<class FieldKind>
class FieldAccessor<FieldKind, double>
{
public:
    typedef float NativeType;

    static NativeType getValue(typename FieldKind::ContextJType context, jfieldID fieldId)
    {
        return TypeConversion<NativeType>::takeOwnershipOfJavaValueAndConvertToNative( FieldKind::getDoubleField(context, fieldId) );
    }

    static void setValue(typename FieldKind::ContextJType context, jfieldID fieldId, const NativeType& value)
    {
        std::list<Reference> createdJavaObjects;
        return FieldKind::setDoubleField( context, fieldId, TypeConversion<NativeType>::nativeToJava(value, createdJavaObjects) );
    }
};



}
}


#endif


