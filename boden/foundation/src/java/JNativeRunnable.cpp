#include <bdn/init.h>
#include <bdn/java/JNativeRunnable.h>

#include <bdn/java/Env.h>
#include <bdn/entry.h>

extern "C" JNIEXPORT void JNICALL Java_io_boden_java_NativeRunnable_nativeRun(
    JNIEnv *pEnv, jobject rawSelf, jobject rawNativeObject)
{
    BDN_ENTRY_BEGIN(pEnv);

    bdn::java::JNativeStrongPointer nativePointer(
        (bdn::java::Reference::convertExternalLocal(rawNativeObject)));

    bdn::IBase *pBase = nativePointer.getPointer_();

    bdn::ISimpleCallable *pCallable =
        dynamic_cast<bdn::ISimpleCallable *>(pBase);

    pCallable->call();

    BDN_ENTRY_END();
}
