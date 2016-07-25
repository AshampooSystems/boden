#include <bdn/init.h>
#include <bdn/android/JNativeRootView.h>

#include <bdn/java/Env.h>

#include <bdn/android/WindowCore.h>
#include <bdn/android/JContext.h>


extern "C" JNIEXPORT void JNICALL Java_io_boden_android_NativeRootView_created(JNIEnv* pEnv, jobject rawSelf, jobject rawContext )
{
    BDN_JNI_BEGIN(pEnv);

    bdn::java::LocalReference self(rawSelf);

    bdn::android::JContext context( bdn::java::LocalReference(rawContext) );

    bdn::android::WindowCore::_registerRootView( self, context, context.getResources().getConfiguration() );

    BDN_JNI_END;
}

extern "C" JNIEXPORT void JNICALL Java_io_boden_android_NativeRootView_disposed( JNIEnv* pEnv, jobject rawSelf)
{
    BDN_JNI_BEGIN(pEnv);

    bdn::java::Reference self(rawSelf);

    bdn::android::WindowCore::_deregisterRootView( self );

    BDN_JNI_END;
}


extern "C" JNIEXPORT void JNICALL Java_io_boden_android_NativeRootView_sizeChanged(JNIEnv* pEnv, jobject rawSelf, int newWidth, int newHeight)
{
    BDN_JNI_BEGIN(pEnv);

    bdn::java::Reference self(rawSelf);

    bdn::android::WindowCore::_rootViewSizeChanged( self, newWidth, newHeight );

    BDN_JNI_END;
}



extern "C" JNIEXPORT void JNICALL Java_io_boden_android_NativeRootView_configurationChanged(JNIEnv* pEnv, jobject rawSelf, jobject rawNewConfig)
{
    BDN_JNI_BEGIN(pEnv);

    bdn::java::Reference self(rawSelf);

    JavaConfiguration newConfig( Reference(rawNewConfig) );

    WindowCore::_rootViewConfigurationChanged( self, newConfig );

    BDN_JNI_END;
}

