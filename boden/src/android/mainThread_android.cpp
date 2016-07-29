#include <bdn/init.h>
#include <bdn/mainThread.h>

#include <bdn/android/JNativeHandler.h>
#include <bdn/java/JNativeOnceRunnable.h>

namespace bdn
{

void CallFromMainThreadBase_::dispatch()
{
    bdn::android::JNativeHandler    handler = bdn::android::JNativeHandler::getMainNativeHandler();

    bdn::java::JNativeOnceRunnable  runnable( this );

    handler.post( runnable );
}



}






