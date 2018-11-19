#include <bdn/init.h>
#include <bdn/java/Env.h>

#include <bdn/java/JavaException.h>
#include <bdn/java/JNativeException.h>

extern "C" JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved)
{
    bdn::java::Env::_onLoad(vm);

    return JNI_VERSION_1_6;
}

namespace bdn
{
    namespace java
    {

        BDN_SAFE_STATIC_THREAD_LOCAL_IMPL(Env, Env::get)

        JavaVM *Env::_globalVm = nullptr;

        Env::Env()
        {
            _envIfKnown = nullptr;
            _mustDetachThread = false;
        }

        Env::~Env()
        {
            // the env object is destroyed automatically when the thread exits
            // (since it is thread local).
            if (_mustDetachThread) {
                _globalVm->DetachCurrentThread();
                _mustDetachThread = false;
            }
        }

        void Env::_onLoad(JavaVM *vm) { _globalVm = vm; }

        void Env::setEnv(JNIEnv *env, bool mustDetachThread)
        {
            if (_envIfKnown == nullptr) {
                _envIfKnown = env;
                _mustDetachThread = mustDetachThread;
            }
        }

        void Env::createEnv()
        {
            if (_envIfKnown == nullptr) {
                if (_globalVm == nullptr) {
                    throw std::runtime_error("bdn::java::Env used from unattached thread, but "
                                             "global VM pointer not initialized.");
                }

                JNIEnv *env = nullptr;
                bool mustDetachThread = false;
                int getResult = _globalVm->GetEnv((void **)&env, JNI_VERSION_1_6);
                if (getResult == JNI_EDETACHED) {
                    // thread is not yet attached to the VM. Attach it.
                    int attachResult = _globalVm->AttachCurrentThread(&env, nullptr);
                    if (attachResult != JNI_OK) {
                        throw std::runtime_error("Fatal error: unable to attach JNI to current "
                                                 "thread (JavaVM::AttachCurrentThread result " +
                                                 std::to_string(attachResult) + ").");
                    }

                    mustDetachThread = true;
                } else if (getResult != JNI_OK) {
                    throw std::runtime_error("Fatal error: unable to get JNI env for current thread "
                                             "(JNIEnv::GetEnv result " +
                                             std::to_string(getResult) + ".");
                }

                setEnv(env, mustDetachThread);
            }
        }

        void Env::throwAndClearExceptionFromLastJavaCall()
        {
            JNIEnv *env = getJniEnv();

            jthrowable exc = env->ExceptionOccurred();
            env->ExceptionClear();
            if (exc != nullptr)
                JavaException::rethrowThrowable(JThrowable(Reference::convertAndDestroyOwnedLocal((jobject)exc)));
        }

        void Env::setJavaSideException(const std::exception_ptr &exceptionPtr)
        {
            JNIEnv *env = getJniEnv();

            try {
                std::rethrow_exception(exceptionPtr);
            }
            catch (JavaException &e) {
                // this is a wrapped java exception. Throw the original
                // exception.
                env->Throw((jthrowable)e.getJThrowable_().getRef_().getJObject());
            }
            catch (...) {
                // a real c++ exception. Wrap it in a dummy java exception and
                // throw that.
                bdn::java::JNativeException nativeException(exceptionPtr);
                env->Throw((jthrowable)nativeException.getRef_().getJObject());
            }
        }
    }
}
