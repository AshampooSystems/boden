#pragma once

#include <bdn/java/JObject.h>

namespace bdn
{
    namespace java
    {

        /** Accessor for Java java.lang.Throwable objects.*/
        class JThrowable : public JObject
        {
          public:
            /** @param javaRef the reference to the Java object.
             *      The JObject instance will copy this reference and keep its
             * type. So if you want the JObject instance to hold a strong
             * reference then you need to call toStrong() on the reference first
             * and pass the result.
             *      */
            explicit JThrowable(const Reference &javaRef) : JObject(javaRef) {}

            /** Returns the detail message (=error message) of the throwable.*/
            String getMessage()
            {
                static MethodId methodId;

                return invoke_<String>(getStaticClass_(), methodId, "getMessage");
            }

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
                static JClass cls("java/lang/Throwable");

                return cls;
            }

            JClass &getClass_() override { return getStaticClass_(); }
        };
    }
}
