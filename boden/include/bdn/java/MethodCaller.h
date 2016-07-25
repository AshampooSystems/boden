#ifndef BDN_JAVA_MethodCaller_H_
#define BDN_JAVA_MethodCaller_H_


#include <bdn/java/TypeConversion.h>

namespace bdn
{
namespace java
{

class MethodCallerBase_
{
public:

protected:
    static jobject  callObjectMethod(jobject obj, jmethodID methodId, ...);
    static void     callVoidMethod(jobject obj, jmethodID methodId, ...);
    static jint     callIntMethod(jobject obj, jmethodID methodId, ...);
    static jboolean callBooleanMethod(jobject obj, jmethodID methodId, ...);
    static jshort   callShortMethod(jobject obj, jmethodID methodId, ...);
    static jlong    callLongMethod(jobject obj, jmethodID methodId, ...);
    static jchar    callCharMethod(jobject obj, jmethodID methodId, ...);
    static jfloat   callFloatMethod(jobject obj, jmethodID methodId, ...);
    static jdouble  callDoubleMethod(jobject obj, jmethodID methodId, ...);
};


/** Calls Java-side functions with the return type specified by the
 *  ReturnType template argument.
 *
 *  This is implemented via template specializations for the various possible
 *  return types.
 *
 *  The class is used like this:
 *
 *  \code
 *
 *  bool returnValue = MethodCaller<bool>::call( instanceJObject, methodId, arg1, arg2, ...);
 *
 *  \endcode
 *
 *  The call() method can handle any number of arguments of any type for which a native-java type conversion
 *  is defined (see TypeConversion).
 *
 *  */
template<typename ReturnType>
class MethodCaller : public MethodCallerBase_
{
public:


    /** Calls the specified method of the object instance obj.
     *
     * You can pass any number of arguments of any type for which a native-java type conversion
     * is defined (see TypeConversion). The arguments and return types must match the definition
     * of the Java-side method, of course.
    **/
    template<typename... Arguments>
    static ReturnType call(jobject obj, jmethodID methodId, Arguments... args)
    {
        return javaToNative( callObjectMethod(obj, methodId, nativeToJava(args)...) );
    }
};


template<>
class MethodCaller<void> : public MethodCallerBase_
{
public:

    template<typename... Arguments>
    static void call(jobject obj, jmethodID methodId, Arguments... args)
    {
        callVoidMethod(obj, methodId, nativeToJava(args)...);
    }
};





template<>
class MethodCaller<bool> : public MethodCallerBase_
{
public:

    template<typename... Arguments>
    static bool call(jobject obj, jmethodID methodId, Arguments... args)
    {
        return javaToNative( callBoolMethod(obj, methodId, nativeToJava(args)... ) );
    }
};


template<>
class MethodCaller<jobject> : public MethodCallerBase_
{
public:

    template<typename... Arguments>
    static jobject call(jobject obj, jmethodID methodId, Arguments... args)
    {
        return javaToNative( callObjectMethod(obj, methodId, nativeToJava(args)... ) );
    }

};

template<>
class MethodCaller<int8_t> : public MethodCallerBase_
{
public:

    template<typename... Arguments>
    static int8_t call(jobject obj, jmethodID methodId, Arguments... args)
    {
        return javaToNative( callByteMethod(obj, methodId, nativeToJava(args)... ) );
    }

};

template<>
class MethodCaller<char32_t> : public MethodCallerBase_
{
public:

    template<typename... Arguments>
    static char32_t call(jobject obj, jmethodID methodId, Arguments... args)
    {
        return javaToNative( callCharMethod(obj, methodId, nativeToJava(args)... ) );
    }

};


template<>
class MethodCaller<int> : public MethodCallerBase_
{
public:

    template<typename... Arguments>
    static int call(jobject obj, jmethodID methodId, Arguments... args)
    {
        return javaToNative( callIntMethod(obj, methodId, nativeToJava(args)... ) );
    }

};


template<>
class MethodCaller<short> : public MethodCallerBase_
{
public:

    template<typename... Arguments>
    static short call(jobject obj, jmethodID methodId, Arguments... args)
    {
        return javaToNative( callShortMethod(obj, methodId, nativeToJava(args)... ) );
    }

};

template<>
class MethodCaller<int64_t> : public MethodCallerBase_
{
public:

    template<typename... Arguments>
    static int64_t call(jobject obj, jmethodID methodId, Arguments... args)
    {
        return javaToNative( callLongMethod(obj, methodId, nativeToJava(args)... ) );
    }
};


template<>
class MethodCaller<float> : public MethodCallerBase_
{
public:

    template<typename... Arguments>
    static float call(jobject obj, jmethodID methodId, Arguments... args)
    {
        return javaToNative( callFloatMethod(obj, methodId, nativeToJava(args)... ) );
    }
};



template<>
class MethodCaller<double> : public MethodCallerBase_
{
public:

    template<typename... Arguments>
    static double call(jobject obj, jmethodID methodId, Arguments... args)
    {
        return javaToNative( callDoubleMethod(obj, methodId, nativeToJava(args)... ) );
    }
};



}
}


#endif


