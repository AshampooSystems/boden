#include <bdn/init.h>
#include <bdn/android/appEntry.h>

#include <bdn/android/AppRunner.h>

#include <bdn/entry.h>

namespace bdn
{
namespace android
{


void appEntry(const std::function< P<AppControllerBase>() >& appControllerCreator, JNIEnv* pEnv )
{
    BDN_ENTRY_BEGIN( pEnv );

    bdn::P< bdn::android::AppRunner > pAppRunner = bdn::newObj< bdn::android::AppRunner >( appControllerCreator );
    _setAppRunner(pAppRunner);
    
    pAppRunner->entry();

    BDN_ENTRY_END();
}


}
}


