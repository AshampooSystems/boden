#ifndef BDN_ANDROID_JavaNativeWeakPointer_H_
#define BDN_ANDROID_JavaNativeWeakPointer_H_

#include <bdn/java/JObject.h>
#include <bdn/java/JByteBuffer.h>

namespace bdn
{
namespace java
{


/** Convenience class that enables simple storing of a C++ pointer in a java object.
 *
 *  There actually is no NativeWeakPointer class on the java side. On the java side
 *  a java.nio.ByteBuffer object is used.  NativeWeakPointer only simplifies
 *  the use of these objects.
 *
 * */
class NativeWeakPointer : public JObject
{
private:
    static Reference newInstance(void* pPointer)
    {
        if(pPointer==nullptr)
        {
            // When the C++ pointer is null then we just return a null java reference
            return Reference();
        }
        else
        {
            // wrap the pointer into a java byte buffer
            JByteBuffer byteBuffer( pPointer, 1);

            return byteBuffer.getRef_();
        }
    }

public:
    explicit NativeWeakPointer(void* pPointer)
     : JObject( newInstance(pPointer) )
    {
    }

    /** @param objectRef the reference to the Java object.
    *      The JObject instance will copy this reference and keep its type.
    *      So if you want the JObject instance to hold a strong reference
    *      then you need to call toStrong() on the reference first and pass the result.
    *      */
    explicit NativeWeakPointer(const Reference& objectRef)
    : JObject(objectRef)
    {
    }


    /** An optimized function to retrieve the stored pointer directly from the specified
     *  jobject. The java-side object must have been created with NativeWeakPointer.*/
    static void* unwrapJObject(jobject obj)
    {
        Env& env = Env::get();

        void* pBuffer = env.getJniEnv()->GetDirectBufferAddress( obj );

        env.throwAndClearExceptionFromLastJavaCall();

        return pBuffer;
    }


    void* getPointer()
    {
        Reference bufferRef = getRef_();

        if(bufferRef.isNull())
        {
            // that means that the C++ pointer is null.
            return nullptr;
        }
        else
        {
            JByteBuffer buffer( bufferRef );

            return buffer.getBuffer_();
        }
    }


    /** Returns the JClass object for this class.
     *
     *  This is the same class object as that JavaByteBuffer, since the Java-side peer
     *  object is simply a plain ByteBuffer instance.
     *  */
    static JClass& getStaticClass_ ()
    {
        return JByteBuffer::getStaticClass_();
    }

    JClass& getClass_() override
    {
        return getStaticClass_();
    }
};


}
}


#endif


