#pragma once

#include <bdn/java/Env.h>
#include <bdn/java/wrapper/Object.h>

namespace bdn::java::wrapper
{
    /** Wrapper for Java java.nio.ByteBuffer objects.
     *
     *  Creating a new object of this type is mainly useful for these use
     cases:
     *
        1) to have Java directly manipulate the referenced memory.

        2) to simply wrap a C++ pointer as a Java object, with the intention
           of storing it in a Java object and then retrieving it later.

        If the memory is not interpreted by Java code then you can pass
        a dummy value (e.g. 1) for the buffer size.
     *
     * */
    class ByteBuffer : public Object
    {
      private:
        Reference newInstance_(void *buffer, int capacityBytes)
        {
            Env &env = Env::get();

            jobject obj = env.getJniEnv()->NewDirectByteBuffer(buffer, capacityBytes);
            env.throwAndClearExceptionFromLastJavaCall();

            return Reference::convertAndDestroyOwnedLocal(obj);
        }

      public:
        /** @param objectRef the reference to the Java object.
         *      The JObject instance will copy this reference and keep its
         * type. So if you want the JObject instance to hold a strong
         * reference then you need to call toStrong() on the reference first
         * and pass the result.
         *      */
        explicit ByteBuffer(const Reference &ref) : Object(ref) {}

        /** Creates a new byte buffer on the java side, which references the
         * specified memory block.
         *
         *  In some cases the intention of creating the Java buffer is
         * simply to store a memory address on the Java side. If only the
         * memory address is relevant to your use case and the memory is not
         * accessed on the Java side then then you can set the capacity to a
         * dummy value >0 (e.g. 1).
         *
         *  */
        ByteBuffer(void *buffer, int64_t capacityBytes) : Object(newInstance_(buffer, capacityBytes)) {}

        /** Return a pointer to the buffer data.*/
        void *getBuffer_()
        {
            Env &env = Env::get();

            void *buffer = env.getJniEnv()->GetDirectBufferAddress(getJObject_());
            env.throwAndClearExceptionFromLastJavaCall();

            return buffer;
        }

        /** Returns the size of the buffer in bytes.*/
        int64_t getCapacityBytes_()
        {
            Env &env = Env::get();

            int64_t bytes = env.getJniEnv()->GetDirectBufferCapacity(getJObject_());
            env.throwAndClearExceptionFromLastJavaCall();

            return bytes;
        }

        /** Returns the java class object for the Java object's class.
         *
         *  Note that the returned class object is not necessarily unique
         * for the whole process. You might get different objects if this
         * function is called from different shared libraries.
         *
         *  If you want to check for type equality then you should compare
         * the type name (see getTypeName() )
         *  */
        static Class &getStaticClass_()
        {
            static Class cls("java/nio/ByteBuffer");

            return cls;
        }

        Class &getClass_() override { return getStaticClass_(); }
    };
}
