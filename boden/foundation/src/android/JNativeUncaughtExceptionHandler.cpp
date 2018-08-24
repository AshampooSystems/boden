#include <bdn/init.h>

#include <bdn/entry.h>
#include <bdn/java/JThrowable.h>
#include <bdn/java/JavaException.h>

#include <bdn/ExceptionReference.h>


extern "C" JNIEXPORT jboolean JNICALL Java_io_boden_android_NativeUncaughtExceptionHandler_nativeUncaughtException(JNIEnv* pEnv, jclass rawCls, jobject rawThrowable, jboolean canKeepRunning )
{
    BDN_ENTRY_BEGIN(pEnv);

    try
    {
        bdn::java::JThrowable throwable( bdn::java::Reference::convertExternalLocal(rawThrowable) );

        bdn::java::JavaException::rethrowThrowable(throwable);
    }
    catch(...)
    {
        // note that we can always keep running on android.
        return bdn::unhandledException( canKeepRunning!=JNI_FALSE ) ? JNI_TRUE : JNI_FALSE;
    }

    BDN_ENTRY_END();


    // should never reach here.
    return JNI_FALSE;
}



