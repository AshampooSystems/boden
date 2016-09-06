#ifndef BDN_JAVA_JObject_H_
#define BDN_JAVA_JObject_H_

namespace bdn
{
namespace java
{

class JObject;
class JClass;       // include for this is below

}
}

#include <bdn/java/MethodId.h>
#include <bdn/java/Reference.h>

namespace bdn
{
namespace java
{

/** Accessor class for a java object.
 *
 *  Conceptually all the J... classes are accessor classes that allow easy access to
 *  Java-side objects of the corresponding class.
 *
 *  The J... objects are very light-weight and can be used for temporary short-lived access
 *  to a Java object. There is no need to optimize or cache JObject instance creation. You
 *  can just create a new object whenever you need to access a Java-side object.
 *
 *  Typically the J... objects expose a subset of the methods of the real Java object.
 *  New method wrappers can be added easily, though.
 *
 *  The naming conventions of JObject methods are simple:
 *
 *  If the name ends with an underscore then the method is a utility function
 *  that does not correspond directly to a Java method.
 *
 *  If the name does not end in an underscore then the method calls the corresponding
 *  java method directly and behaves the same way as the Java method (unless documented
 *  otherwise).
 *
 * */
class JObject : public Base
{
public:
    /** @param javaRef the reference to the Java object.
     *      The JObject instance will copy this reference and keep its type.
     *      So if you want the JObject instance to hold a strong reference
     *      then you need to call toStrong() on the reference first and pass the result.
     *      */
    explicit JObject(const Reference& javaRef)
     : _javaRef( javaRef )
    {
    }


    /** copy constructor*/
    JObject(const JObject& o)
     : _javaRef( o._javaRef )
    {
    }

    JObject()
    {
    }


    /** Returns the Reference to the Java-side object.
     *
     *  Note that the returned reference can be a null Reference object.
     * */
    Reference getRef_ ()
    {
        return _javaRef;
    }


    /** Returns the JNI jobject value that is associated
     *  with this JObject instance. Same as getRef_().getJObject()
     * */
    jobject getJObject_ ()
    {
        return _javaRef.getJObject();
    }



    /** Releases the reference to the java-side object.
     *  Sets the internally stored java reference to null.*/
    void releaseJavaReference_ ()
    {
        _javaRef = Reference();
    }


    /** Retrieves the actual, dynamically determined class name of
     *  this object. The name is returned in canonical notation
     *  (e.g. "java.lang.String").
     *
     *  */
    String getCanonicalClassName_ ();


    /** Returns true if the object is an instance of the specified java class.*/
    bool isInstanceOf_ (JClass& cls);


    /** Returns true if the Java object reference associated with this instance is null.*/
    bool isNull_()
    {
        return getRef_().isNull();
    }


    template<typename ReturnType, typename... Arguments>
    inline ReturnType invoke_(JClass& cls, MethodId& methodId, const String& methodName, Arguments... args);

    template<typename ReturnType, typename... Arguments>
    inline static ReturnType invokeStatic_(JClass& cls, MethodId& methodId, const String& methodName, Arguments... args);


    /** Returns the JClass object for this class.
     *
     *  Note that the returned class object is not necessarily unique for the whole
     *  process.
     *  You might get different objects if this function is called from different
     *  shared libraries.
     *
     *  If you want to check for type equality then you should compare the type name
     *  (see getTypeName() )
     *  */
    static JClass& getStaticClass_();


    /** Returns the JClass object that represents the java class that corresponds to this object's C++ class.
     *
     *  Note that this does NOT dynamically look up the Java-side class of the Java-side object.
     *  This simply returns the Java class that corresponds to the C++ class of the JObject instance.
     *
     *  Subclasses of JObject must override this to return the correct class.
     * */
    virtual JClass& getClass_ ()
    {
        return getStaticClass_ ();
    }

private:
    Reference _javaRef;
};


}
}


#include <bdn/java/JClass.h>



namespace bdn
{
namespace java
{


template<typename ReturnType, typename... Arguments>
inline ReturnType JObject::invoke_(JClass& cls, MethodId& methodId, const String& methodName, Arguments... args)
{
    return cls.invokeObjectMethod_<ReturnType, Arguments...>( methodId, getJObject_(), methodName, args...);
}


template<typename ReturnType, typename... Arguments>
inline ReturnType JObject::invokeStatic_(JClass& cls, MethodId& methodId, const String& methodName, Arguments... args)
{
    return cls.invokeStaticMethod_<ReturnType, Arguments...>( methodId, methodName, args...);
}


}
}

#endif


