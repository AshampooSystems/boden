#pragma once

#include <bdn/java/Env.h>
#include <bdn/java/wrapper/CharSequence.h>

namespace bdn::java::wrapper
{
    /** Wrapper for Java java.lang.String objects.*/
    class String : public CharSequence
    {
      private:
        static Reference newInstance_(const bdn::String &s)
        {
            Env &env = Env::get();

            jstring obj = env.getJniEnv()->NewStringUTF(s.c_str());
            env.throwAndClearExceptionFromLastJavaCall();

            return Reference::convertAndDestroyOwnedLocal((jobject)obj);
        }

      public:
        /** @param objectRef the reference to the Java object.
         *      The JObject instance will copy this reference and keep its
         * type. So if you want the JObject instance to hold a strong
         * reference then you need to call toStrong() on the reference first
         * and pass the result.
         *      */
        explicit String(const Reference &objectRef) : CharSequence(objectRef) {}

        String(const bdn::String &inString) : String(newInstance_(inString)) {}

        bdn::String getValue_()
        {
            JNIEnv *env = Env::get().getJniEnv();
            jstring javaRef = (jstring)getJObject_();

            const char *data = env->GetStringUTFChars(javaRef, nullptr);

            // note that GetStringUTFChars does not throw any java-side
            // exceptions.

            bdn::String val(data);

            env->ReleaseStringUTFChars(javaRef, data);

            return val;
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
        static Class &getStaticClass_()
        {
            static Class cls("java/lang/String");

            return cls;
        }

        Class &getClass_() override { return getStaticClass_(); }
    };
}

namespace bdn
{
    using JavaString = bdn::java::wrapper::String;
}
