#include <bdn/init.h>
#include <bdn/mainThread.h>

#include <bdn/android/JNativeHandler.h>
#include <bdn/android/IdleRunner.h>

#include <bdn/java/JNativeOnceRunnable.h>

namespace bdn
{

void CallFromMainThreadBase_::dispatchCall()
{
    bdn::android::JNativeHandler    handler = bdn::android::JNativeHandler::getMainNativeHandler();

    bdn::java::JNativeOnceRunnable  runnable( this );

    handler.post( runnable );
}

void CallFromMainThreadBase_::dispatchCallWhenIdle()
{
    bdn::android::IdleRunner::get().callOnceWhenIdle(this);
}

void CallFromMainThreadBase_::dispatchCallWithDelaySeconds(double seconds)
{
    int64_t millis = (int64_t)(seconds*1000);

    if(millis<=0)
        dispatchCall();
    else
    {
        bdn::android::JNativeHandler handler = bdn::android::JNativeHandler::getMainNativeHandler();

        bdn::java::JNativeOnceRunnable runnable(this);

        handler.postDelayed(runnable, millis);
    }
}



}






