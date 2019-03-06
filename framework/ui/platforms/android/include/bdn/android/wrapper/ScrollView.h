#pragma once

#include <bdn/android/wrapper/Context.h>
#include <bdn/android/wrapper/FrameLayout.h>
#include <bdn/java/wrapper/String.h>

namespace bdn::android::wrapper
{

    /** Accessor for Java android.widget.ScrollView objects.*/
    class ScrollView : public FrameLayout
    {
      private:
        static bdn::java::Reference newInstance_(Context context)
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
        explicit ScrollView(const bdn::java::Reference &objectRef) : FrameLayout(objectRef) {}

        ScrollView(Context context) : ScrollView(newInstance_(context)) {}

        /** Set the scrolled position of your view. This will cause a call
           to onScrollChanged(int, int, int, int) and the view will be
           invalidated.

            This version also clamps the scrolling to the bounds of our
           child.
         */
        void scrollTo(int x, int y)
        {
            static bdn::java::MethodId methodId;

            invoke_<void>(getStaticClass_(), methodId, "scrollTo", x, y);
        }

        /** Like scrollTo(int, int), but scroll smoothly instead of
         * immediately. */
        void smoothScrollTo(int x, int y)
        {
            static bdn::java::MethodId methodId;

            invoke_<void>(getStaticClass_(), methodId, "smoothScrollTo", x, y);
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
        static bdn::java::wrapper::Class &getStaticClass_()
        {
            static bdn::java::wrapper::Class cls("android/widget/ScrollView");

            return cls;
        }

        bdn::java::wrapper::Class &getClass_() { return getStaticClass_(); }
    };
}
