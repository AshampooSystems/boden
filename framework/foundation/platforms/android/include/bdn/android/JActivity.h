#ifndef BDN_ANDROID_JActivity_H_
#define BDN_ANDROID_JActivity_H_

#include <bdn/android/JContext.h>

namespace bdn
{
    namespace android
    {

        /** Accessor for Java android.app.Activity objects.*/
        class JActivity : public JContext
        {
          public:
            /** @param javaRef the reference to the Java object.
             *      The JObject instance will copy this reference and keep its
             * type. So if you want the JObject instance to hold a strong
             * reference then you need to call toStrong() on the reference first
             * and pass the result.
             *      */
            explicit JActivity(const bdn::java::Reference &javaRef)
                : JContext(javaRef)
            {}

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
                static bdn::java::JClass cls("android/app/Activity");

                return cls;
            }

            bdn::java::JClass &getClass_() override
            {
                return getStaticClass_();
            }
        };
    }
}

#endif
