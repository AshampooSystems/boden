#pragma once

#include <bdn/android/wrapper/ViewGroup.h>

namespace bdn::android::wrapper
{

    /** Wrapper for Java android.widget.FrameLayout objects.*/
    class FrameLayout : public JViewGroup
    {
      public:
        /** @param javaRef the reference to the Java object.
         *      The JObject instance will copy this reference and keep its
         * type. So if you want the JObject instance to hold a strong
         * reference then you need to call toStrong() on the reference first
         * and pass the result.
         *      */
        explicit FrameLayout(const bdn::java::Reference &javaRef) : JViewGroup(javaRef) {}

        /** Returns the JClass object for this class.
         *
         *  Note that the returned class object is not necessarily unique
         * for the whole process. You might get different objects if this
         * function is called from different shared libraries.
         *
         *  If you want to check for type equality then you should compare
         * the type name (see getTypeName() )
         *  */
        static bdn::java::wrapper::Class &getStaticClass_()
        {
            static bdn::java::wrapper::Class cls("android/widget/FrameLayout");

            return cls;
        }

        bdn::java::wrapper::Class &getClass_() override { return getStaticClass_(); }

        /** Wrapper for Java android.widget.FrameLayout.LayoutParams
         * objects.
         *
         * */
        class LayoutParams : public JViewGroup::JMarginLayoutParams
        {
          private:
            static bdn::java::Reference newInstance_(int width, int height, int gravity)
            {
                static bdn::java::MethodId constructorId;

                return getStaticClass_().newInstance_(constructorId, width, height, gravity);
            }

          public:
            /** @param javaRef the reference to the Java object.
             *      The JObject instance will copy this reference and keep
             * its type. So if you want the JObject instance to hold a
             * strong reference then you need to call toStrong() on the
             * reference first and pass the result.
             *      */
            explicit LayoutParams(const bdn::java::Reference &javaRef) : JViewGroup::JMarginLayoutParams(javaRef) {}

            enum
            {
                /** Value for gravity indicating that a gravity has not been
                 * explicitly specified.
                 *
                 *  It is treated by FrameLayout as Gravity.TOP |
                 * Gravity.START.
                 * */
                UNSPECIFIED_GRAVITY = -1
            };

            LayoutParams(int width, int height, int gravity = UNSPECIFIED_GRAVITY)
                : JViewGroup::JMarginLayoutParams(newInstance_(width, height, gravity))
            {}

            /** Returns the JClass object for this class.
             *
             *  Note that the returned class object is not necessarily
             * unique for the whole process. You might get different objects
             * if this function is called from different shared libraries.
             *
             *  If you want to check for type equality then you should
             * compare the type name (see getTypeName() )
             *  */
            static bdn::java::wrapper::Class &getStaticClass_()
            {
                static bdn::java::wrapper::Class cls("android/widget/FrameLayout$LayoutParams");

                return cls;
            }

            bdn::java::wrapper::Class &getClass_() override { return getStaticClass_(); }
        };
    };
}
