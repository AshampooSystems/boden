#ifndef BDN_JAVA_Env_H_
#define BDN_JAVA_Env_H_

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
            BDN_SAFE_STATIC_THREAD_LOCAL(Env, get);

            /** Indicates that a block of native code was begun.
             *
             *  This is called automatically by the BDN_ENTRY_BEGIN macro. It is
             * usually not necessary to call this directly.
             *
             *  pEnv must be a pointer to the JNI environment object that was
             * received by the JNI function.*/
            void jniBlockBegun(JNIEnv *pEnv)
            {
                // note that it is safe to cache the pointer since the Env
                // objects are all thread local (and the JNIEnv stays the same
                // for the same thread).
                if (_pEnvIfKnown == nullptr)
                    setEnv(pEnv, false);
            }

            /** Returns the Jni environment object.
             *  Throws a JniEnvNotSetError if it has not been set yet.
             *  The JNI environment is set with the macro BDN_ENTRY_BEGIN.
             **/
            JNIEnv *getJniEnv()
            {
                if (_pEnvIfKnown == nullptr)
                    createEnv();
                return _pEnvIfKnown;
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
            static void _onLoad(JavaVM *pVm);

          private:
            Env();
            friend class RawNewAllocator_Base_<Env>;

            void createEnv();
            void setEnv(JNIEnv *pEnv, bool mustDetachThread);

            JNIEnv *_pEnvIfKnown;
            bool _mustDetachThread;

            static JavaVM *_pGlobalVm;
        };
    }
}

#endif
