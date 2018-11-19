#ifndef BDN_ANDROID_JInputMethodManager_H_
#define BDN_ANDROID_JInputMethodManager_H_

#include <bdn/java/JObject.h>
#include <bdn/android/JIBinder.h>

namespace bdn
{
    namespace android
    {
        /** Accessor for Java android.view.inputmethod.InputMethodManager objects.
         */
        class JInputMethodManager : public bdn::java::JObject
        {
          public:
            /** @param javaRef the reference to the Java object.
             *      The JObject instance will copy this reference and keep its type.
             *      So if you want the JObject instance to hold a strong reference
             *      then you need to call toStrong() on the reference first and pass the
             * result.
             *      */
            explicit JInputMethodManager(const bdn::java::Reference &javaRef) : JObject(javaRef) {}

            /** Synonym for hideSoftInputFromWindow(IBinder, int, ResultReceiver) without
                a result: request to hide the soft input window from the context of the
                window that is currently accepting input.*/
            bool hideSoftInputFromWindow(JIBinder windowToken, int flags)
            {
                static bdn::java::MethodId methodId;

                return invoke_<bool>(getStaticClass_(), methodId, "hideSoftInputFromWindow", windowToken, flags);
            }

            /** Returns the JClass object for this class.
             *
             *  Note that the returned class object is not necessarily unique for the
             * whole process. You might get different objects if this function is called
             * from different shared libraries.
             *
             *  If you want to check for type equality then you should compare the type
             * name (see getTypeName() )
             *  */
            static bdn::java::JClass &getStaticClass_()
            {
                static bdn::java::JClass cls("android/view/inputmethod/InputMethodManager");

                return cls;
            }

            bdn::java::JClass &getClass_() override { return getStaticClass_(); }
        };
    }
}

#endif