#include <bdn/init.h>
#include <bdn/java/JNativeStrongPointer.h>

#include <bdn/android/JavaConnector.h>

#include <jni.h>


extern "C" JNIEXPORT void JNICALL Java_io_boden_android_NativeStrongPointer_disposed(JNIEnv* pEnv, jobject rawSelf, jobject rawByteBuffer)
{
    BDN_ANDROID_JNI_BEGIN(pEnv);

    JavaByteBuffer byteBuffer( Reference(rawByteBuffer) );

    IBase* pObject = static_cast<IBase*>( byteBuffer.getBuffer() );

    pObject->releaseRef();

    BDN_ANDROID_JNI_END;
}




