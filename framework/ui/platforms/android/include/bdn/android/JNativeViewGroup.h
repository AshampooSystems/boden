#ifndef BDN_ANDROID_JNativeViewGroup_H_
#define BDN_ANDROID_JNativeViewGroup_H_

#include <bdn/android/JViewGroup.h>

namespace bdn
{
    namespace android
    {

        /** Accessor for Java-side io.boden.android.NativeViewGroup objects.*/
        class JNativeViewGroup : public JViewGroup
        {
          private:
            bdn::java::Reference newInstance_(JContext context)
            {
                static bdn::java::MethodId constructorId;

                return bdn::java::Reference(
                    getStaticClass_().newInstance_(constructorId, context));
            }

          public:
            explicit JNativeViewGroup(JContext context)
                : JNativeViewGroup(newInstance_(context))
            {}

            /** @param objectRef the reference to the Java object.
             *      The JObject instance will copy this reference and keep its
             * type. So if you want the JObject instance to hold a strong
             * reference then you need to call toStrong() on the reference first
             * and pass the result.
             *      */
            explicit JNativeViewGroup(const bdn::java::Reference &javaRef)
                : JViewGroup(javaRef)
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
                static bdn::java::JClass cls(
                    "io/boden/android/NativeViewGroup");

                return cls;
            }

            void setSize(int width, int height)
            {
                static bdn::java::MethodId methodId;

                invoke_<void>(getStaticClass_(), methodId, "setSize", width,
                              height);
            }

            void setChildBounds(JView &childView, int x, int y, int width,
                                int height)
            {
                static bdn::java::MethodId methodId;

                invoke_<void>(getStaticClass_(), methodId, "setChildBounds",
                              childView, x, y, width, height);
            }
        };
    }
}

#endif
