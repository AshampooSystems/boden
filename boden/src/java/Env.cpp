#include <bdn/init.h>
#include <bdn/java/Env.h>


extern "C" JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* pVm, void* reserved)
{
    bdn::java::Env::_onLoad(pVm);

    return JNI_VERSION_1_6;
}


namespace bdn
{
namespace java
{

BDN_SAFE_STATIC_THREAD_LOCAL_IMPL( Env, Env::get )

JavaVM* Env::_pGlobalVm = nullptr;


Env::Env()
{
    _pEnvIfKnown = nullptr;
    _mustDetachThread = false;
}


Env::~Env()
{
    // the env object is destroyed automatically when the thread exits (since
    // it is thread local).
    if(_mustDetachThread)
    {
        _pGlobalVm->DetachCurrentThread();
        _mustDetachThread = false;
    }
}


void Env::_onLoad(JavaVM* pVm)
{
    _pGlobalVm = pVm;
}


void Env::setEnv(JNIEnv* pEnv, bool mustDetachThread)
{
    if(_pEnvIfKnown==nullptr)
    {
        _pEnvIfKnown = pEnv;
        _mustDetachThread = mustDetachThread;
    }
}

void Env::createEnv()
{
    if(_pEnvIfKnown==nullptr)
    {
        if (_pGlobalVm == nullptr)
        {
            throw std::runtime_error(
                    "bdn::java::Env used from unattached thread, but global VM pointer not initialized.");
        }

        JNIEnv* pEnv = nullptr;
        bool    mustDetachThread = false;
        int getResult = _pGlobalVm->GetEnv((void**)&pEnv, JNI_VERSION_1_6);
        if (getResult == JNI_EDETACHED)
        {
            // thread is not yet attached to the VM. Attach it.
            int attachResult = _pGlobalVm->AttachCurrentThread(&pEnv, nullptr);
            if (attachResult != JNI_OK)
            {
                throw std::runtime_error(
                        "Fatal error: unable to attach JNI to current thread (JavaVM::AttachCurrentThread result " +
                        std::to_string(attachResult) + ").");
            }

            mustDetachThread = true;
        }
        else if (getResult != JNI_OK)
        {
            throw std::runtime_error(
                    "Fatal error: unable to get JNI env for current thread (JNIEnv::GetEnv result " +
                    std::to_string(getResult) + ".");
        }

        setEnv( pEnv, mustDetachThread );
    }
}





}
}






