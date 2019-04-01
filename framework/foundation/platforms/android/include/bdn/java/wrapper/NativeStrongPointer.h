#pragma once

#include <bdn/java/wrapper/ByteBuffer.h>
#include <bdn/java/wrapper/Object.h>

namespace bdn::java::wrapper
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
    class NativeStrongPointer : public Object
    {
      private:
        static Reference newInstance_(const std::shared_ptr<void> &pObject);

      public:
        explicit NativeStrongPointer(const std::shared_ptr<void> &pObject);

        /** @param objectRef the reference to the Java object.
         *      The JObject instance will copy this reference and keep its
         * type. So if you want the JObject instance to hold a strong
         * reference then you need to call toStrong() on the reference first
         * and pass the result.
         *      */
        explicit NativeStrongPointer(const Reference &objectRef);

        ByteBuffer getWrappedPointer();

        std::shared_ptr<void> getPointer();
        std::shared_ptr<bdn::Base> getBdnBasePointer();

        /*
        static Base *unwrapJObject(jobject obj);
        */

        /** Returns the JClass object for this class.
         *
         *  Note that the returned class object is not necessarily unique
         * for the whole process. You might get different objects if this
         * function is called from different shared libraries.
         *
         *  If you want to check for type equality then you should compare
         * the type name (see getTypeName() )
         *  */
        static Class &getStaticClass_();

        Class &getClass_() override { return getStaticClass_(); }
    };
}

namespace bdn::java
{
    template <> class TypeConversion<std::shared_ptr<bdn::Base>> : public TypeConversionBase_
    {
      public:
        using JavaType = jobject;
        using NativeType = std::shared_ptr<bdn::Base>;

        static String getJavaSignature()
        {
            static String s("Lio/boden/java/NativeStrongPointer;");
            return s;
        }

        static JavaType nativeToJava(const NativeType &arg, std::list<Reference> &createdJavaObjects)
        {

            wrapper::NativeStrongPointer javaPtr(arg);
            Reference ref = javaPtr.getRef_();
            createdJavaObjects.push_back(ref);
            return ref.getJObject();
        }

        static NativeType takeOwnershipOfJavaValueAndConvertToNative(JavaType arg)
        {
            wrapper::NativeStrongPointer native(bdn::java::Reference::convertExternalLocal(arg));
            return std::static_pointer_cast<bdn::Base>(native.getPointer());
        }
    };

    template <class Actual> class TypeConversion<std::shared_ptr<Actual>> : public TypeConversionBase_
    {
      public:
        using JavaType = jobject;
        using NativeType = std::shared_ptr<Actual>;

        static String getJavaSignature()
        {
            static String s("Lio/boden/java/NativeStrongPointer;");
            return s;
        }

        static JavaType nativeToJava(const NativeType &arg, std::list<Reference> &createdJavaObjects)
        {
            wrapper::NativeStrongPointer javaPtr(std::static_pointer_cast<bdn::Base>(arg));
            Reference ref = javaPtr.getRef_();
            createdJavaObjects.push_back(ref);
            return ref.getJObject();
        }

        static NativeType takeOwnershipOfJavaValueAndConvertToNative(JavaType arg)
        {
            wrapper::NativeStrongPointer native(bdn::java::Reference::convertExternalLocal(arg));
            return std::dynamic_pointer_cast<Actual>(native.getPointer());
        }
    };
}
