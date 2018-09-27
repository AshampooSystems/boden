#include <bdn/init.h>
#include <bdn/android/WindowCore.h>

namespace bdn
{
    namespace android
    {

        BDN_SAFE_STATIC_THREAD_LOCAL_IMPL(
            WindowCore::RootViewRegistry,
            WindowCore::getRootViewRegistryForCurrentThread);
    }
}
