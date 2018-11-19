#include <bdn/init.h>
#include <bdn/java/JNativeRunnable.h>

#include <bdn/java/Env.h>
#include <bdn/entry.h>

extern "C" JNIEXPORT void JNICALL Java_io_boden_java_NativeRunnable_nativeRun(JNIEnv *env, jobject rawSelf,
                                                                              jobject rawNativeObject)
{
    bdn::platformEntryWrapper(
        [&]() {
            bdn::java::JNativeStrongPointer nativePointer(
                (bdn::java::Reference::convertExternalLocal(rawNativeObject)));

            bdn::IBase *base = nativePointer.getPointer_();

            bdn::ISimpleCallable *callable = dynamic_cast<bdn::ISimpleCallable *>(base);

            callable->call();
        },
        true, env);
}
