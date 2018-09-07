#include <bdn/init.h>
#include <bdn/pthread/ThreadLocalStorageManager.h>

namespace bdn
{
    namespace pthread
    {

        BDN_SAFE_STATIC_IMPL(ThreadLocalStorageManager,
                             ThreadLocalStorageManager::get);
    }
}
