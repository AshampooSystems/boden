#pragma once

#include <bdn/Base.h>

#include <bdn/jni.h>

namespace bdn::java
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
        ~Env() override;

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
            if (_envIfKnown == nullptr) {
                setEnv(env, false);
            }
        }

        /** Returns the Jni environment object.
         *  Throws a JniEnvNotSetError if it has not been set yet.
         *  The JNI environment is set with the macro BDN_ENTRY_BEGIN.
         **/
        JNIEnv *getJniEnv()
        {
            if (_envIfKnown == nullptr) {
                createEnv();
            }
            return _envIfKnown;
        }

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
