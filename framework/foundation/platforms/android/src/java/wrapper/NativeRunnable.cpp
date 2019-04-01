
#include <bdn/java/wrapper/NativeRunnable.h>

#include <bdn/entry.h>
#include <bdn/java/Env.h>

extern "C" JNIEXPORT void JNICALL Java_io_boden_java_NativeRunnable_nativeRun(JNIEnv *pEnv, jobject rawSelf,
                                                                              jobject rawNativeObject)
{
    bdn::platformEntryWrapper(
        [&]() {
            bdn::java::wrapper::NativeStrongPointer nativePointer(
                (bdn::java::Reference::convertExternalLocal(rawNativeObject)));

            auto pCallable = std::static_pointer_cast<std::function<void()>>(nativePointer.getPointer());

            (*pCallable)();
        },
        true, pEnv);
}
