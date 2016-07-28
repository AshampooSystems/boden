#ifndef BDN_JAVA_FieldAccessor_H_
#define BDN_JAVA_FieldAccessor_H_


#include <bdn/java/TypeConversion.h>

namespace bdn
{
namespace java
{

class FieldAccessorBase_
{
public:

protected:
    static jobject  getObjectField(jobject obj, jfieldID fieldId);
    static jboolean getBooleanField(jobject obj, jfieldID fieldId);
    static jint     getIntField(jobject obj, jfieldID fieldId);
    static jshort   getShortField(jobject obj, jfieldID fieldId);
    static jbyte    getByteField(jobject obj, jfieldID fieldId);
    static jlong    getLongField(jobject obj, jfieldID fieldId);
    static jchar    getCharField(jobject obj, jfieldID fieldId);
    static jfloat   getFloatField(jobject obj, jfieldID fieldId);
    static jdouble  getDoubleField(jobject obj, jfieldID fieldId);


    static void setObjectField(jobject obj, jfieldID fieldId, jobject value);
    static void setBooleanField(jobject obj, jfieldID fieldId, jboolean value);
    static void setIntField(jobject obj, jfieldID fieldId, jint value);
    static void setShortField(jobject obj, jfieldID fieldId, jshort value);
    static void setLongField(jobject obj, jfieldID fieldId, jlong value);
    static void setByteField(jobject obj, jfieldID fieldId, jbyte value);
    static void setCharField(jobject obj, jfieldID fieldId, jchar value);
    static void setFloatField(jobject obj, jfieldID fieldId, jfloat value);
    static void setDoubleField(jobject obj, jfieldID fieldId, jdouble value);

};


/** Allows access to fields of java-side objects.
 *
 *  Usage:
 *
 *  \code
 *
 *  jobject  obj = ...;
 *  jfieldID fieldID = ...;
 *  int      newValue = ...;
 *
 *  int oldVal = FieldAccessor<int>::getValue(obj, fieldId);
 *
 *  FieldAccessor<int>::setValue(obj, fieldId, newValue);
 *
 *  \endcode
 **/
template<typename NATIVE_TYPE>
class FieldAccessor : public FieldAccessorBase_
{
public:
    typedef NATIVE_TYPE NativeType;

    static NativeType getValue(jobject obj, jfieldID fieldId)
    {
        return TypeConversion<NativeType>::takeOwnershipOfJavaValueAndConvertToNative( getObjectField(obj, fieldId) );
    }

    static void setValue(jobject obj, jfieldID fieldId, const NativeType& value)
    {
        std::list<Reference> createdJavaObjects;
        return setObjectField( obj, fieldId, TypeConversion<NativeType>::nativeToJava(value, createdJavaObjects) );
    }
};



template<>
class FieldAccessor<bool> : public FieldAccessorBase_
{
public:
    typedef bool NativeType;

    static NativeType getValue(jobject obj, jfieldID fieldId)
    {
        return TypeConversion<NativeType>::takeOwnershipOfJavaValueAndConvertToNative( getBooleanField(obj, fieldId) );
    }

    static void setValue(jobject obj, jfieldID fieldId, NativeType value)
    {
        std::list<Reference> createdJavaObjects;
        return setBooleanField( obj, fieldId, TypeConversion<NativeType>::nativeToJava(value, createdJavaObjects) );
    }
};


template<>
class FieldAccessor<int> : public FieldAccessorBase_
{
public:
    typedef int NativeType;

    static NativeType getValue(jobject obj, jfieldID fieldId)
    {
        return TypeConversion<NativeType>::takeOwnershipOfJavaValueAndConvertToNative( getIntField(obj, fieldId) );
    }

    static void setValue(jobject obj, jfieldID fieldId, const NativeType& value)
    {
        std::list<Reference> createdJavaObjects;
        return setIntField( obj, fieldId, TypeConversion<NativeType>::nativeToJava(value, createdJavaObjects) );
    }
};


template<>
class FieldAccessor<short> : public FieldAccessorBase_
{
public:
    typedef short NativeType;

    static NativeType getValue(jobject obj, jfieldID fieldId)
    {
        return TypeConversion<NativeType>::takeOwnershipOfJavaValueAndConvertToNative( getShortField(obj, fieldId) );
    }

    static void setValue(jobject obj, jfieldID fieldId, const NativeType& value)
    {
        std::list<Reference> createdJavaObjects;
        return setShortField( obj, fieldId, TypeConversion<NativeType>::nativeToJava(value, createdJavaObjects) );
    }
};


template<>
class FieldAccessor<int64_t> : public FieldAccessorBase_
{
public:
    typedef int64_t NativeType;

    static NativeType getValue(jobject obj, jfieldID fieldId)
    {
        return TypeConversion<NativeType>::takeOwnershipOfJavaValueAndConvertToNative( getLongField(obj, fieldId) );
    }

    static void setValue(jobject obj, jfieldID fieldId, const NativeType& value)
    {
        std::list<Reference> createdJavaObjects;
        return setLongField( obj, fieldId, TypeConversion<NativeType>::nativeToJava(value, createdJavaObjects) );
    }
};


template<>
class FieldAccessor<char32_t> : public FieldAccessorBase_
{
public:
    typedef char32_t NativeType;

    static NativeType getValue(jobject obj, jfieldID fieldId)
    {
        return TypeConversion<NativeType>::takeOwnershipOfJavaValueAndConvertToNative( getCharField(obj, fieldId) );
    }

    static void setValue(jobject obj, jfieldID fieldId, const NativeType& value)
    {
        std::list<Reference> createdJavaObjects;
        return setCharField( obj, fieldId, TypeConversion<NativeType>::nativeToJava(value, createdJavaObjects) );
    }
};


template<>
class FieldAccessor<int8_t> : public FieldAccessorBase_
{
public:
    typedef int8_t NativeType;

    static NativeType getValue(jobject obj, jfieldID fieldId)
    {
        return TypeConversion<NativeType>::takeOwnershipOfJavaValueAndConvertToNative( getByteField(obj, fieldId) );
    }

    static void setValue(jobject obj, jfieldID fieldId, const NativeType& value)
    {
        std::list<Reference> createdJavaObjects;
        return setByteField( obj, fieldId, TypeConversion<NativeType>::nativeToJava(value, createdJavaObjects) );
    }
};



template<>
class FieldAccessor<float> : public FieldAccessorBase_
{
public:
    typedef float NativeType;

    static NativeType getValue(jobject obj, jfieldID fieldId)
    {
        return TypeConversion<NativeType>::takeOwnershipOfJavaValueAndConvertToNative( getFloatField(obj, fieldId) );
    }

    static void setValue(jobject obj, jfieldID fieldId, const NativeType& value)
    {
        std::list<Reference> createdJavaObjects;
        return setFloatField( obj, fieldId, TypeConversion<NativeType>::nativeToJava(value, createdJavaObjects) );
    }
};


template<>
class FieldAccessor<double> : public FieldAccessorBase_
{
public:
    typedef float NativeType;

    static NativeType getValue(jobject obj, jfieldID fieldId)
    {
        return TypeConversion<NativeType>::takeOwnershipOfJavaValueAndConvertToNative( getDoubleField(obj, fieldId) );
    }

    static void setValue(jobject obj, jfieldID fieldId, const NativeType& value)
    {
        std::list<Reference> createdJavaObjects;
        return setDoubleField( obj, fieldId, TypeConversion<NativeType>::nativeToJava(value, createdJavaObjects) );
    }
};



}
}


#endif


