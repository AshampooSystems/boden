#ifndef BDN_ANDROID_JNativeEditTextTextWatcher_H_
#define BDN_ANDROID_JNativeEditTextTextWatcher_H_

#include <bdn/java/JObject.h>
#include <bdn/android/JTextView.h>
#include <bdn/android/JTextWatcher.h>

namespace bdn
{
    namespace android
    {

        /** Accessor for Java-side io.boden.android.NativeEditTextTextWatcher
         * objects.*/
        class JNativeEditTextTextWatcher : public JTextWatcher
        {
          private:
            static bdn::java::Reference newInstance_(JTextView &textView)
            {
                static bdn::java::MethodId constructorId;
                return getStaticClass_().newInstance_(constructorId, textView);
            }

          public:
            JNativeEditTextTextWatcher(JTextView &textView)
                : JNativeEditTextTextWatcher(newInstance_(textView))
            {}

            /** @param objectRef the reference to the Java object.
             *      The JObject instance will copy this reference and keep its
             * type. So if you want the JObject instance to hold a strong
             * reference then you need to call toStrong() on the reference first
             * and pass the result.
             *      */
            explicit JNativeEditTextTextWatcher(
                const bdn::java::Reference &objectRef)
                : JTextWatcher(objectRef)
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
                    "io/boden/android/NativeEditTextTextWatcher");
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
