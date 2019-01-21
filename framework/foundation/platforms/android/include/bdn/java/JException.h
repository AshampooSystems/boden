#pragma once

#include <bdn/java/JThrowable.h>

namespace bdn
{
    namespace java
    {

        /** Accessor for Java java.lang.Exception objects.*/
        class JException : public JThrowable
        {
          private:
            static Reference newInstance_(const String &message)
            {
                static MethodId constructorId;

                return getStaticClass_().newInstance_(constructorId, message);
            }

          public:
            /** @param javaRef the reference to the Java object.
             *      The JObject instance will copy this reference and keep its
             * type. So if you want the JObject instance to hold a strong
             * reference then you need to call toStrong() on the reference first
             * and pass the result.
             *      */
            explicit JException(const Reference &javaRef) : JThrowable(javaRef) {}

            explicit JException(const String &message) : JException(newInstance_(message)) {}

            /** Returns the JClass object for this class.
             *
             *  Note that the returned class object is not necessarily unique
             * for the whole process. You might get different objects if this
             * function is called from different shared libraries.
             *
             *  If you want to check for type equality then you should compare
             * the type name (see getTypeName() )
             *  */
            static JClass &getStaticClass_()
            {
                static JClass cls("java/lang/Exception");

                return cls;
            }

            JClass &getClass_() override { return getStaticClass_(); }
        };
    }
}
