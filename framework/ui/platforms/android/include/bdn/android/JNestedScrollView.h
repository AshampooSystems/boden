#pragma once

#include <bdn/android/JViewGroup.h>
#include <bdn/android/JContext.h>
#include <bdn/java/JString.h>

namespace bdn
{
    namespace android
    {

        /** Accessor for Java android.support.v4.widget.NestedScrollView
         * objects.*/
        class JNestedScrollView : public JViewGroup
        {
          private:
            static bdn::java::Reference newInstance_(JContext context)
            {
                static bdn::java::MethodId constructorId;

                return getStaticClass_().newInstance_(constructorId, context);
            }

          public:
            /** @param objectRef the reference to the Java object.
             *      The JObject instance will copy this reference and keep its
             * type. So if you want the JObject instance to hold a strong
             * reference then you need to call toStrong() on the reference first
             * and pass the result.
             *      */
            explicit JNestedScrollView(const bdn::java::Reference &objectRef) : JViewGroup(objectRef) {}

            JNestedScrollView(JContext context) : JNestedScrollView(newInstance_(context)) {}

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
                static bdn::java::JClass cls("android/support/v4/widget/NestedScrollView");

                return cls;
            }

            bdn::java::JClass &getClass_() { return getStaticClass_(); }
        };
    }
}
