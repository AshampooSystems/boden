#include <bdn/init.h>
#include <bdn/android/JNativeRootView.h>

#include <bdn/java/Env.h>

#include <bdn/android/WindowCore.h>
#include <bdn/android/JContext.h>


extern "C" JNIEXPORT void JNICALL Java_io_boden_android_NativeRootView_created(JNIEnv* pEnv, jobject rawSelf )
{
    BDN_JNI_BEGIN(pEnv);

    bdn::java::ExternalLocalReference self(rawSelf);

    bdn::android::WindowCore::_rootViewCreated( self );

    BDN_JNI_END;
}

extern "C" JNIEXPORT void JNICALL Java_io_boden_android_NativeRootView_disposed( JNIEnv* pEnv, jobject rawSelf)
{
    BDN_JNI_BEGIN(pEnv);

    bdn::java::ExternalLocalReference self(rawSelf);

    bdn::android::WindowCore::_rootViewDisposed( self );

    BDN_JNI_END;
}


extern "C" JNIEXPORT void JNICALL Java_io_boden_android_NativeRootView_sizeChanged(JNIEnv* pEnv, jobject rawSelf, int newWidth, int newHeight)
{
    BDN_JNI_BEGIN(pEnv);

    bdn::java::ExternalLocalReference self(rawSelf);

    bdn::android::WindowCore::_rootViewSizeChanged( self, newWidth, newHeight );

    BDN_JNI_END;
}



extern "C" JNIEXPORT void JNICALL Java_io_boden_android_NativeRootView_configurationChanged(JNIEnv* pEnv, jobject rawSelf, jobject rawNewConfig)
{
    BDN_JNI_BEGIN(pEnv);

    bdn::java::ExternalLocalReference self(rawSelf);

    bdn::android::JConfiguration newConfig(( bdn::java::ExternalLocalReference(rawNewConfig) ));

    bdn::android::WindowCore::_rootViewConfigurationChanged( self, newConfig );

    BDN_JNI_END;
}

