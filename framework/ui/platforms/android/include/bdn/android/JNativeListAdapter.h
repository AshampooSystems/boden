#pragma once

#include <bdn/java/JObject.h>
#include <bdn/android/JView.h>
#include <bdn/android/JBaseAdapter.h>

namespace bdn
{
    namespace android
    {

        /** Accessor for Java-side io.boden.android.NativeListAdapter
         * objects.*/
        class JNativeListAdapter : public JBaseAdapter
        {
          private:
            static bdn::java::Reference newInstance_(JView &view)
            {
                static bdn::java::MethodId constructorId;
                return getStaticClass_().newInstance_(constructorId, view);
            }

          public:
            JNativeListAdapter(JView &view) : JNativeListAdapter(newInstance_(view)) {}

            /** @param objectRef the reference to the Java object.
             *      The JObject instance will copy this reference and keep its
             * type. So if you want the JObject instance to hold a strong
             * reference then you need to call toStrong() on the reference first
             * and pass the result.
             *      */
            explicit JNativeListAdapter(const bdn::java::Reference &objectRef) : JBaseAdapter(objectRef) {}

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
                static bdn::java::JClass cls("io/boden/android/NativeListAdapter");
                return cls;
            }

            bdn::java::JClass &getClass_() override { return getStaticClass_(); }
        };
    }
}
