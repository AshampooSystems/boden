#include <bdn/init.h>
#include <bdn/win32/ThreadLocalStorageManager.h>

namespace bdn
{
    namespace win32
    {

        BDN_SAFE_STATIC_IMPL(ThreadLocalStorageManager,
                             ThreadLocalStorageManager::get);
    }
}
