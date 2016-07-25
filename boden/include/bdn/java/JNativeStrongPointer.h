#ifndef BDN_JAVA_JNativeStrongPointer_H_
#define BDN_JAVA_JNativeStrongPointer_H_

#include <bdn/java/JObject.h>
#include <bdn/java/JByteBuffer.h>

namespace bdn
{
namespace java
{


/** Accessor for Java-side io.boden.java.NativeStrongPointer objects.
 *
 *  These objects can be used to store a strong pointer to a C++ object
 *  in the java world.
 *
 *  The Java object will hold a reference to the C++ object and keep it alive.
 *  The reference is released when the Java-side object is deleted by the garbage collector
 *  or when dispose is called on the Java-side object.
 *
 *  NativeWeakPointer can be used to store a weak pointer in a Java object.
 *
 * */
class JNativeStrongPointer : public JObject
{
private:
    static Reference newInstance_(IBase* pObject)
    {
        if(pObject==nullptr)
        {
            // When the C++ pointer is null then we just return a null java reference
            return Reference();
        }
        else
        {
            // wrap the pointer into a java byte buffer
            JByteBuffer byteBuffer( static_cast<void*>(pObject), 1);

            static MethodId constructorId;

            Reference ref = getStaticClass_().newInstance_(constructorId, byteBuffer );

            pObject->addRef();

            return ref;
        }
    }

public:
    explicit JNativeStrongPointer(IBase* pObject)
     : JObject( newInstance_(pObject) )
    {
    }

    /** @param objectRef the reference to the Java object.
    *      The JObject instance will copy this reference and keep its type.
    *      So if you want the JObject instance to hold a strong reference
    *      then you need to call toStrong() on the reference first and pass the result.
    *      */
    explicit JNativeStrongPointer(const Reference& objectRef)
    : JObject(objectRef)
    {
    }


    Reference getWrappedPointer()
    {
        static MethodId methodId;

        return Reference( invoke_<JObject>(methodId, "getWrappedPointer" ) ).toAccessible();
    }

    P<IBase> getPointer_ ()
    {
        Reference  bufferRef = getWrappedPointer();

        if(bufferRef.isNull())
        {
            // that means that the C++ pointer is null.
            return nullptr;
        }
        else
        {
            JByteBuffer buffer( bufferRef );

            IBase* p = static_cast<IBase*>( buffer.getBuffer_() );

            return p;
        }
    }


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
    static JClass& getStaticClass()
    {
        static JClass cls( "io/boden/java/NativeStrongPointer" );

        return cls;
    }

};


}
}


#endif


