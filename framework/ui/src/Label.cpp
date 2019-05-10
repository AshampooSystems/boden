
#include <bdn/ui/Label.h>

namespace bdn::ui
{
    namespace detail
    {
        VIEW_CORE_REGISTRY_IMPLEMENTATION(Label)
    }

    Label::Label(std::shared_ptr<ViewCoreFactory> viewCoreFactory) : View(std::move(viewCoreFactory)), wrap(true)
    {
        detail::VIEW_CORE_REGISTER(Label, View::viewCoreFactory());
    }

    void Label::bindViewCore()
    {
        View::bindViewCore();
        auto textCore = View::core<Label::Core>();
        textCore->text.bind(text);
        textCore->wrap.bind(wrap);
    }
}
