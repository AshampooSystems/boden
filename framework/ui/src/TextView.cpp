
#include <bdn/TextView.h>

namespace bdn
{
    namespace detail
    {
        VIEW_CORE_REGISTRY_IMPLEMENTATION(TextView)
    }

    TextView::TextView(std::shared_ptr<ViewCoreFactory> viewCoreFactory) : View(std::move(viewCoreFactory)), wrap(true)
    {
        detail::VIEW_CORE_REGISTER(TextView, View::viewCoreFactory());
    }

    void TextView::bindViewCore()
    {
        View::bindViewCore();
        auto textCore = View::core<TextViewCore>();
        textCore->text.bind(text);
        textCore->wrap.bind(wrap);
    }
}
