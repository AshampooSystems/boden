#ifndef BDN_ANDROID_appEntry_H_
#define BDN_ANDROID_appEntry_H_

#include <bdn/AppControllerBase.h>

#include <jni.h>

namespace bdn
{
namespace android
{
    

/** Entry point function for android apps.
    This is automatically called by the BDN_APP_INIT macro and it usually
    does not need to be called manually.*/
void appEntry(const std::function< P<AppControllerBase>() >& appControllerCreator, JNIEnv* pEnv);


}
}

#endif

