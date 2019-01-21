#pragma once

#include <bdn/java/JObject.h>
#include <bdn/android/JView.h>

namespace bdn
{
    namespace android
    {

        /** Accessor for Java-side io.boden.android.NativeViewCoreClickListener
         * objects.*/
        class JNativeViewCoreClickListener : public JView::OnClickListener
        {
          private:
            static bdn::java::Reference newInstance_()
            {
                static bdn::java::MethodId constructorId;

                return getStaticClass_().newInstance_(constructorId);
            }

          public:
            JNativeViewCoreClickListener() : JNativeViewCoreClickListener(newInstance_()) {}

            /** @param objectRef the reference to the Java object.
             *      The JObject instance will copy this reference and keep its
             * type. So if you want the JObject instance to hold a strong
             * reference then you need to call toStrong() on the reference first
             * and pass the result.
             *      */
            explicit JNativeViewCoreClickListener(const bdn::java::Reference &objectRef)
                : JView::OnClickListener(objectRef)
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
                static bdn::java::JClass cls("io/boden/android/NativeViewCoreClickListener");

                return cls;
            }

            bdn::java::JClass &getClass_() override { return getStaticClass_(); }
        };
    }
}
