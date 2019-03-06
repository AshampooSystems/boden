
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

            std::shared_ptr<bdn::Base> p = nativePointer.getPointer_();

            std::shared_ptr<bdn::ISimpleCallable> pCallable = std::dynamic_pointer_cast<bdn::ISimpleCallable>(p);

            pCallable->call();
        },
        true, pEnv);
}
