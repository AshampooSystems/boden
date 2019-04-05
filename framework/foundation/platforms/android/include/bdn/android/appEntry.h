#pragma once

#include <bdn/Application.h>
#include <bdn/jni.h>

namespace bdn::android
{

    /** Entry point function for android apps.
        This is automatically called by the BDN_APP_INIT macro and it
       usually does not need to be called manually.*/
    void appEntry(const Application::ApplicationControllerFactory &appControllerCreator, JNIEnv *env,
                  jobject rawIntent);
}
