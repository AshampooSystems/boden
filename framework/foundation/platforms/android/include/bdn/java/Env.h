#pragma once

#include <bdn/Base.h>

#include <bdn/java/JniEnvNotSetError.h>

#include <jni.h>

namespace bdn
{
    namespace java
    {

        /** Used to access the java environment. Wraps a JNIEnv instance.
         *
         *  Each Env object can only be used from a single thread. Because of
         * this it is discouraged to store pointers to Env objects for later
         * use.
         *
         *  Instead you can use the static get() function to access the object
         * for the current thread.
         *
         */
        class Env : public Base
        {
          public:
            ~Env();

            /** Static function that returns a reference to the Env instance for
             * the current thread. The instance cannot be used in other threads.
             *  */
            static Env &get();

            /** Indicates that a block of native code was begun.
             *
             *  This is called automatically by the BDN_ENTRY_BEGIN macro. It is
             * usually not necessary to call this directly.
             *
             *  env must be a pointer to the JNI environment object that was
             * received by the JNI function.*/
            void jniBlockBegun(JNIEnv *env)
            {
                // note that it is safe to cache the pointer since the Env
                // objects are all thread local (and the JNIEnv stays the same
                // for the same thread).
                if (_envIfKnown == nullptr)
                    setEnv(env, false);
            }

            /** Returns the Jni environment object.
             *  Throws a JniEnvNotSetError if it has not been set yet.
             *  The JNI environment is set with the macro BDN_ENTRY_BEGIN.
             **/
            JNIEnv *getJniEnv()
            {
                if (_envIfKnown == nullptr)
                    createEnv();
                return _envIfKnown;
            }

            /** If the last JAVA call produced an exception then this is thrown
             * as a C++ exception and the exception is cleared from the global
             * state. Since the exception is cleared, a second call to
             * throwAndClearExceptionFromLastJavaCall() will return without any
             * exception.
             *
             *  If the Java exception is a wrapped C++ exception then the
             * original C++ exception is thrown.
             * */
            void throwAndClearExceptionFromLastJavaCall();

            /** "Throws" an exception on the java side. A java exception object
             * is created and stored as the result of the currently active JNI
             * call. Once the JNI function returns that exception will be thrown
             * on the java side.
             *
             *  If the exception is a wrapped java exception then the original
             * java exception is thrown on the java side.
             *
             *  */
            void setJavaSideException(const std::exception_ptr &exceptionPtr);

            /** Used internally. Do not call.*/
            static void _onLoad(JavaVM *vm);

          private:
            Env();

            void createEnv();
            void setEnv(JNIEnv *env, bool mustDetachThread);

            JNIEnv *_envIfKnown;
            bool _mustDetachThread;

            static JavaVM *_globalVm;
        };
    }
}
