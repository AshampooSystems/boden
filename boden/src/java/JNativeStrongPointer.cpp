#include <bdn/init.h>
#include <bdn/java/JNativeStrongPointer.h>

#include <bdn/java/Env.h>


extern "C" JNIEXPORT void JNICALL Java_io_boden_java_NativeStrongPointer_disposed(JNIEnv* pEnv, jobject rawSelf, jobject rawByteBuffer)
{
    BDN_JNI_BEGIN(pEnv);

    bdn::java::JByteBuffer byteBuffer(( bdn::java::Reference::convertExternalLocal(rawByteBuffer) ));

    bdn::IBase* pObject = static_cast<bdn::IBase*>( byteBuffer.getBuffer_() );

    pObject->releaseRef();

    BDN_JNI_END;
}




