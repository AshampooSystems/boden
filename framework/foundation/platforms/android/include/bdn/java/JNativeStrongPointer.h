#pragma once

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
         *  The Java object will hold a reference to the C++ object and keep it
         * alive. The reference is released when the Java-side object is deleted
         * by the garbage collector or when dispose is called on the Java-side
         * object.
         *
         *  NativeWeakPointer can be used to store a weak pointer in a Java
         * object.
         *
         * */
        class JNativeStrongPointer : public JObject
        {
          private:
            static Reference newInstance_(std::shared_ptr<Base> pObject);

          public:
            explicit JNativeStrongPointer(std::shared_ptr<Base> pObject);

            /** @param objectRef the reference to the Java object.
             *      The JObject instance will copy this reference and keep its
             * type. So if you want the JObject instance to hold a strong
             * reference then you need to call toStrong() on the reference first
             * and pass the result.
             *      */
            explicit JNativeStrongPointer(const Reference &objectRef);

            JByteBuffer getWrappedPointer();

            std::shared_ptr<Base> getPointer_();

            /** An optimized function to retrieve the stored pointer directly
             * from the specified jobject. The java-side object must be a
             * ByteBuffer as returned by getWrappedPointer().*/
            static Base *unwrapJObject(jobject obj);

            /** Returns the JClass object for this class.
             *
             *  Note that the returned class object is not necessarily unique
             * for the whole process. You might get different objects if this
             * function is called from different shared libraries.
             *
             *  If you want to check for type equality then you should compare
             * the type name (see getTypeName() )
             *  */
            static JClass &getStaticClass_();

            JClass &getClass_() override { return getStaticClass_(); }
        };
    }
}
