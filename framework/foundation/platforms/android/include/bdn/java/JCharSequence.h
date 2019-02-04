#pragma once

#include <bdn/java/JObject.h>

namespace bdn
{
    namespace java
    {

        /** Accessor for Java java.lang.CharSequence objects.*/
        class JCharSequence : public JObject
        {
          public:
            JCharSequence(const String &str);

            /** @param objectRef the reference to the Java object.
             *      The JObject instance will copy this reference and keep its
             * type. So if you want the JObject instance to hold a strong
             * reference then you need to call toStrong() on the reference first
             * and pass the result.
             *      */
            explicit JCharSequence(const Reference &ref) : JObject(ref) {}

            /** Returns the java class object for the Java object's class.
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
                static JClass cls("java/lang/CharSequence");

                return cls;
            }

            JClass &getClass_() override { return getStaticClass_(); }

            operator String() { return toString(); }
        };
    }
}
