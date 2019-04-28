#pragma once

#include <bdn/java/wrapper/ByteBuffer.h>
#include <bdn/java/wrapper/Object.h>

#include <memory>
#include <utility>

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
        static Reference newInstance(const std::weak_ptr<void> &weakPtr)
        {
            if (weakPtr.expired()) {
                // When the C++ pointer is null then we just return a null
                // java reference
                return Reference();
            }

            // NOLINTNEXTLINE(cppcoreguidelines-owning-memory)
            auto *pPtr = new std::weak_ptr<void>(weakPtr);

            // wrap the pointer into a java byte buffer
            ByteBuffer byteBuffer(pPtr, 1);

            return byteBuffer.getRef_();
        }

      public:
        explicit NativeWeakPointer(const std::weak_ptr<void> &pPointer) : Object(newInstance(pPointer)) {}

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

            // NOLINTNEXTLINE(cppcoreguidelines-owning-memory, cppcoreguidelines-pro-type-cstyle-cast)
            auto *pPtr = (std::weak_ptr<void> *)env.getJniEnv()->GetDirectBufferAddress(obj);
            return *pPtr;
        }

        std::weak_ptr<void> getPointer()
        {
            Reference bufferRef = getRef_();

            if (bufferRef.isNull()) {
                // that means that the C++ pointer is null.
                return std::weak_ptr<void>();
            }
            ByteBuffer buffer(bufferRef);

            // NOLINTNEXTLINE(cppcoreguidelines-pro-type-cstyle-cast)
            return *((std::weak_ptr<void> *)buffer.getBuffer_());
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
