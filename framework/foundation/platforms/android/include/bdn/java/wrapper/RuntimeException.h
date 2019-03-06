#pragma once

#include <bdn/java/wrapper/Exception.h>

namespace bdn::java::wrapper
{
    /** Accessor for Java java.lang.RuntimeException objects.*/
    class RuntimeException : public Exception
    {
      public:
        /** @param javaRef the reference to the Java object.
         *      The JObject instance will copy this reference and keep its
         * type. So if you want the JObject instance to hold a strong
         * reference then you need to call toStrong() on the reference first
         * and pass the result.
         *      */
        explicit RuntimeException(const Reference &javaRef) : Exception(javaRef) {}

        /** Returns the JClass object for this class.
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
            static Class cls("java/lang/RuntimeException");

            return cls;
        }

        Class &getClass_() override { return getStaticClass_(); }
    };
}
