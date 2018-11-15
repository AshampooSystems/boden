#include <bdn/init.h>
#include <bdn/RequireNewAlloc.h>

namespace bdn
{

    BDN_SAFE_STATIC_THREAD_LOCAL_IMPL(bool, _requireNewAlloc_getThreadLocalAllocatedWithNewRef)
}
