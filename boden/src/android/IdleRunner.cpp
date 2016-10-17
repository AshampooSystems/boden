#include <bdn/init.h>
#include <bdn/android/IdleRunner.h>

#include <bdn/java/Env.h>

#include <bdn/mainThread.h>
#include <bdn/log.h>


extern "C" JNIEXPORT void JNICALL Java_io_boden_android_NativeIdleHandler_nativeOnIdle(JNIEnv* pEnv, jobject rawSelf )
{
    BDN_JNI_BEGIN(pEnv);

    bdn::android::IdleRunner::get()._beginningIdlePhase();

    BDN_JNI_END;
}


namespace bdn
{
namespace android
{

BDN_SAFE_STATIC_IMPL( IdleRunner, IdleRunner::get );

void IdleRunner::callOnceWhenIdle( ISimpleCallable* pCallable)
{
    // if we are currently in idle state then we must break it
    // so that we get a new notification.
    // So instead of locking a mutex and adding the callable directly
    // we post a main thread message and do it there. That makes a Mutex
    // unnecessary and also break any current idle state.

    P<IdleRunner>       pThis = this;
    P<ISimpleCallable>  pCallableWithRef = pCallable;
    asyncCallFromMainThread(
            [pThis, pCallableWithRef]()
            {
                pThis->_callWhenIdleList.push_back(pCallableWithRef);
            }
    );
}


void IdleRunner::_beginningIdlePhase()
{
    // copy the call list so that newly added handlers are not executed
    std::list< P<ISimpleCallable> > toCall = _callWhenIdleList;
    _callWhenIdleList.clear();


    // we want idle tasks to be released immediately after they are executed.
    // So we remove them from the list immediately.
    while(!toCall.empty())
    {
        P<ISimpleCallable>& pCallable = toCall.front();

        try
        {
            pCallable->call();
        }
        catch(std::exception& e)
        {
            // log and ignore
            logError(e, "Exception thrown by idle task in IdleRunner::_beginningIdle. Ignoring.");
        }

        toCall.pop_front();
    }
}

}
}
