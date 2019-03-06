#pragma once

#include <bdn/java/wrapper/ByteBuffer.h>
#include <bdn/java/wrapper/Object.h>

#include <memory>

namespace bdn::java::wrapper
{
    /** Convenience class that enables simple storing of a C++ pointer in a
     * java object.
     *
     *  There actually is no NativeWeakPointer class on the java side. On
     * the java side a java.nio.ByteBuffer object is used. NativeWeakPointer
     * only simplifies the use of these objects.
     *
     * */
    class NativeWeakPointer : public Object
    {
      private:
        static Reference newInstance(std::weak_ptr<void> weakPtr)
        {
            if (weakPtr.expired()) {
                // When the C++ pointer is null then we just return a null
                // java reference
                return Reference();
            } else {
                std::weak_ptr<void> *pPtr = new std::weak_ptr<void>(weakPtr);

                // wrap the pointer into a java byte buffer
                ByteBuffer byteBuffer(pPtr, 1);

                return byteBuffer.getRef_();
            }
        }

      public:
        explicit NativeWeakPointer(std::weak_ptr<void> pPointer) : Object(newInstance(pPointer)) {}

        /** @param objectRef the reference to the Java object.
         *      The JObject instance will copy this reference and keep its
         * type. So if you want the JObject instance to hold a strong
         * reference then you need to call toStrong() on the reference first
         * and pass the result.
         *      */
        explicit NativeWeakPointer(const Reference &objectRef) : Object(objectRef) {}

        /** An optimized function to retrieve the stored pointer directly
         * from the specified jobject. The java-side object must have been
         * created with NativeWeakPointer.*/
        static std::weak_ptr<void> unwrapJObject(jobject obj)
        {
            Env &env = Env::get();

            std::weak_ptr<void> *pPtr = (std::weak_ptr<void> *)env.getJniEnv()->GetDirectBufferAddress(obj);

            env.throwAndClearExceptionFromLastJavaCall();

            return *pPtr;
        }

        std::weak_ptr<void> getPointer()
        {
            Reference bufferRef = getRef_();

            if (bufferRef.isNull()) {
                // that means that the C++ pointer is null.
                return std::weak_ptr<void>();
            } else {
                ByteBuffer buffer(bufferRef);

                return *((std::weak_ptr<void> *)buffer.getBuffer_());
            }
        }

        /** Returns the JClass object for this class.
         *
         *  This is the same class object as that JavaByteBuffer, since the
         * Java-side peer object is simply a plain ByteBuffer instance.
         *  */
        static Class &getStaticClass_() { return ByteBuffer::getStaticClass_(); }

        Class &getClass_() override { return getStaticClass_(); }
    };
}