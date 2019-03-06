

#include <bdn/entry.h>
#include <bdn/java/JavaException.h>
#include <bdn/java/wrapper/Throwable.h>

#include <bdn/ExceptionReference.h>

extern "C" JNIEXPORT jboolean JNICALL Java_io_boden_android_NativeUncaughtExceptionHandler_nativeUncaughtException(
    JNIEnv *env, jclass rawCls, jobject rawThrowable, jboolean canKeepRunning)
{
    jboolean returnValue = JNI_FALSE;

    bdn::platformEntryWrapper(
        [&]() {
            try {
                bdn::java::wrapper::Throwable throwable(bdn::java::Reference::convertExternalLocal(rawThrowable));

                bdn::java::JavaException::rethrowThrowable(throwable);
            }
            catch (...) {
                // note that we can always keep running on android.
                returnValue = bdn::unhandledException(canKeepRunning != JNI_FALSE) ? JNI_TRUE : JNI_FALSE;
            }
        },
        true, env);

    // should never reach here.
    return returnValue;
}
