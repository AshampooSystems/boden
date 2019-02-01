#pragma once

#include <bdn/android/JViewGroup.h>
#include <bdn/android/JNativeListAdapter.h>

namespace bdn
{
    namespace android
    {

        /** Accessor for Java android.widget.ListView objects.*/
        class JListView : public JViewGroup
        {
          private:
            static bdn::java::Reference newInstance_(JContext &context)
            {
                static bdn::java::MethodId constructorId;

                return getStaticClass_().newInstance_(constructorId, context);
            }

          public:
            JListView(JContext &context) : JViewGroup(newInstance_(context)) {}

            /** @param javaRef the reference to the Java object.
             *      The JObject instance will copy this reference and keep its
             * type. So if you want the JObject instance to hold a strong
             * reference then you need to call toStrong() on the reference first
             * and pass the result.
             *      */
            explicit JListView(const bdn::java::Reference &javaRef) : JViewGroup(javaRef) {}

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
                static bdn::java::JClass cls("android/widget/ListView");

                return cls;
            }

            void setAdapter(JNativeListAdapter adapter)
            {
                static bdn::java::MethodId methodId;

                invoke_<void>(getStaticClass_(), methodId, "setAdapter", (JListAdapter)adapter);
            }
        };
    }
}