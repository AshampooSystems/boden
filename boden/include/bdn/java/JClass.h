#ifndef BDN_JAVA_JClass_H_
#define BDN_JAVA_JClass_H_

namespace bdn
{
namespace java
{

class JClass;

}
}


#include <bdn/java/JObject.h>
#include <bdn/java/MethodId.h>
#include <bdn/java/Reference.h>
#include <bdn/java/LocalReference.h>
#include <bdn/java/TypeConversion.h>
#include <bdn/java/MethodCaller.h>


namespace bdn
{
namespace java
{

/** Represents a java class object.*/
class JClass : public JObject
{
private:
    static Reference findClass_ (const String& nameInSlashNotation);

public:
    /** Throws a ClassNotFoundError if the specified class was not found.
     *
     *  @param classNameInSlashNotation the fully qualified JAVA class name
     *      with slashes instead of dots. E.g. java/lang/Object instead of java.lang.Object.*/
    explicit JClass(const String& classNameInSlashNotation)
    : JObject( findClass_ (classNameInSlashNotation).toStrong() )
    {
        _nameInSlashNotation = classNameInSlashNotation;
        _nameInSlashNotationInitialized = true;
    }


    explicit JClass(const Reference& javaRef)
     : JObject( javaRef )
    {
    }



    /** Returns the class object of java.lang.Class*/
    static JClass& getStaticClass_ ()
    {
        static JClass cls("java/lang/Class");

        return cls;
    }

    JClass& getClass_() override
    {
        return getStaticClass_ ();
    }



    /** Returns the canonical name of the java class, as defined by the java language
     *  specification.*/
    String getCanonicalName()
    {
        if(!_canonicalNameInitialized)
        {
            static MethodId methodId;

            _canonicalName = getStaticClass_().invokeObjectMethod_<String>(methodId, getJObject_(), "getCanonicalName" );
            _canonicalNameInitialized = true;
        }

        return _canonicalName;
    }


    /** Returns the name of this class in "slash notation". I.e. slashes are used instead
     *  of dots.*/
    String getNameInSlashNotation_ ()
    {
        if(!_nameInSlashNotationInitialized)
        {
            _nameInSlashNotation = getCanonicalName();
            _nameInSlashNotation.findReplace('.', '/');
            _nameInSlashNotationInitialized = true;
        }

        return _nameInSlashNotation;
    }


    /** Invokes the specified method of the specified class instance.
     *
     *  @param methodId a method id object that is used to cache the method id to make
     *      future calls more efficient. If this is uninitialized then it will automatically
     *      be initialized. If it is initialized then the existing method id will be used.     *
     *  @param obj the Java object instance that the method should be called on
     *  @param name the method name
     *  @param args an arbitrary number of method arguments. If a java object is to be passed to the method then
     *      you should pass a JObject instance here.
     *  @return the return value of the method. The type is determined by the template parameter
     *      ReturnType.
     *  */
    template<typename ReturnType, typename... Arguments>
    ReturnType invokeObjectMethod_ (MethodId &methodId,
                                    jobject obj,
                                    const String& name,
                                    Arguments... args)
    {
        initMethodId<ReturnType, Arguments...>(methodId, name);

        return MethodCaller<ReturnType>::template call<Arguments...>(obj, methodId.getId(), args...);
    }


    /** Creates a new instance of the Java class.
     *
     *  @param constructorId a method id object that is used to cache the constructor's id to make
     *      future calls more efficient. If this is uninitialized then it will automatically
     *      be initialized. If it is initialized then the existing method id will be used.
     *  @param args an arbitrary number of constructor arguments. If a java object is to be passed to the constructor then
     *      you should pass a JObject instance here.
     *  @return jobject reference of the new object instance. Note that this is a local reference, i.e. it will
     *      become invalid after the current JNI call returns. You should wrap it in a JObject to make the reference
     *      permanent.
     *  */
    template<typename... Arguments>
    Reference newInstance_ (MethodId& constructorId, Arguments... args)
    {
        initMethodId<void, Arguments...>(constructorId, "<init>");

        jobject obj = _newObject( (jclass)getJObject_(), constructorId.getId(), nativeToJava(args)... );

        return Reference( obj );
    }

private:


    template<typename FirstType, typename... RemainingTypes>
    static String _makeTypeSignatureList()
    {
        String firstSig = getTypeSignature<FirstType>();

        String remainingSigs = _makeTypeSignatureList<RemainingTypes...>();

        return firstSig+remainingSigs;
    }


    template<typename SingleType>
    static String _makeTypeSignatureList()
    {
        return getTypeSignature<SingleType>();
    }


    template<typename ReturnType, typename... Arguments>
    void initMethodId(MethodId& id, const String& name)
    {
        if(!id.isInitialized())
        {
            String      methodSignature = "("+_makeTypeSignatureList<Arguments...>() + ")" + getTypeSignature<ReturnType>();

            id.init( *this, name, methodSignature );
        }
    }


    template<typename T>
    static String getTypeSignature()
    {
        return TypeConversion<T>::getJavaSignature();
    }


    static jobject _newObject( jclass cls, jmethodID methodId, ... );


    mutable String          _nameInSlashNotation;
    mutable bool            _nameInSlashNotationInitialized = false;
    mutable String          _canonicalName;
    mutable bool            _canonicalNameInitialized = false;
};


}
}


#endif


