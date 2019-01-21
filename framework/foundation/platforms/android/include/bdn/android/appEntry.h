#pragma once

#include <bdn/AppControllerBase.h>

#include <jni.h>

namespace bdn
{
    namespace android
    {

        /** Entry point function for android apps.
            This is automatically called by the BDN_APP_INIT macro and it
           usually does not need to be called manually.*/
        void appEntry(const std::function<std::shared_ptr<AppControllerBase>()> &appControllerCreator, JNIEnv *env,
                      jobject rawIntent);
    }
}
