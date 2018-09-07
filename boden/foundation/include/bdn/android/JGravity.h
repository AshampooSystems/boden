#ifndef BDN_ANDROID_JGravity_H_
#define BDN_ANDROID_JGravity_H_

#include <bdn/java/JObject.h>

namespace bdn
{
    namespace android
    {

        /** Wrapper for Java android.view.Gravity objects.*/
        class JGravity : public bdn::java::JObject
        {
          public:
            /** @param javaRef the reference to the Java object.
             *      The JObject instance will copy this reference and keep its
             * type. So if you want the JObject instance to hold a strong
             * reference then you need to call toStrong() on the reference first
             * and pass the result.
             *      */
            explicit JGravity(const bdn::java::Reference &javaRef)
                : bdn::java::JObject(javaRef)
            {}

            enum
            {
                /**  Flag to clip the edges of the object to its container along
                   the horizontal axis. */
                CLIP_HORIZONTAL = 8,

                /** Flag to clip the edges of the object to its container along
                   the vertical axis. */
                CLIP_VERTICAL = 128
            };

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
                static bdn::java::JClass cls("android/view/Gravity");

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
