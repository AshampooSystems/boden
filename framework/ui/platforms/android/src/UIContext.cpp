#include <bdn/android/ContextWrapper.h>
#include <bdn/ui/UIContext.h>

namespace bdn::ui::android
{
    UIContext::UIContext(std::unique_ptr<android::ContextWrapper> &&contextWrapper)
        : _contextWrapper(std::move(contextWrapper))
    {}
}
