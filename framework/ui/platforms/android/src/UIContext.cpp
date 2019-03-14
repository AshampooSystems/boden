#include <bdn/UIContext.h>
#include <bdn/android/ContextWrapper.h>

namespace bdn::android
{
    UIContext::UIContext(std::unique_ptr<android::ContextWrapper> &&contextWrapper)
        : _contextWrapper(std::move(contextWrapper))
    {}
}
