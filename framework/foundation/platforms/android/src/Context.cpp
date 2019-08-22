#include <bdn/Context.h>
#include <bdn/android/ContextWrapper.h>

namespace bdn::android
{
    Context::Context(std::unique_ptr<ContextWrapper> &&contextWrapper) : _contextWrapper(std::move(contextWrapper)) {}
}
