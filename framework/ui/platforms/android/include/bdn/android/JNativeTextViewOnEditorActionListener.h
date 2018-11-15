#ifndef BDN_ANDROID_JNativeTextViewOnEditorActionListener_H_
#define BDN_ANDROID_JNativeTextViewOnEditorActionListener_H_

#include <bdn/java/JObject.h>
#include <bdn/android/JTextView.h>

namespace bdn
{
    namespace android
    {

        /** Accessor for Java-side
         * io.boden.android.NativeTextViewOnEditorActionListener objects.*/
        class JNativeTextViewOnEditorActionListener : public JTextView::OnEditorActionListener
        {
          private:
            static bdn::java::Reference newInstance_()
            {
                static bdn::java::MethodId constructorId;
                return getStaticClass_().newInstance_(constructorId);
            }

          public:
            JNativeTextViewOnEditorActionListener() : JNativeTextViewOnEditorActionListener(newInstance_()) {}

            /** @param objectRef the reference to the Java object.
             *      The JObject instance will copy this reference and keep its
             * type. So if you want the JObject instance to hold a strong
             * reference then you need to call toStrong() on the reference first
             * and pass the result.
             *      */
            explicit JNativeTextViewOnEditorActionListener(const bdn::java::Reference &objectRef)
                : JTextView::OnEditorActionListener(objectRef)
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
                static bdn::java::JClass cls("io/boden/android/NativeTextViewOnEditorActionListener");
                return cls;
            }

            bdn::java::JClass &getClass_() override { return getStaticClass_(); }
        };
    }
}

#endif
