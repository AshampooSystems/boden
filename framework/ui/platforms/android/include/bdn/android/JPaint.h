#ifndef BDN_ANDROID_JPaint_H_
#define BDN_ANDROID_JPaint_H_

#include <bdn/java/JObject.h>

namespace bdn
{
    namespace android
    {

        /** Accessor for Java 	android.graphics.Paint objects.*/
        class JPaint : public bdn::java::JObject
        {
          public:
            /** @param javaRef the reference to the Java object.
             *      The JObject instance will copy this reference and keep its
             * type. So if you want the JObject instance to hold a strong
             * reference then you need to call toStrong() on the reference first
             * and pass the result.
             *      */
            explicit JPaint(const bdn::java::Reference &javaRef) : JObject(javaRef) {}

            /** Returns the paint's text size.*/
            float getTextSize()
            {
                static bdn::java::MethodId methodId;

                return invoke_<float>(getStaticClass_(), methodId, "getTextSize");
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
            static bdn::java::JClass &getStaticClass_()
            {
                static bdn::java::JClass cls("android/graphics/Paint");

                return cls;
            }

            bdn::java::JClass &getClass_() override { return getStaticClass_(); }
        };
    }
}

#endif
