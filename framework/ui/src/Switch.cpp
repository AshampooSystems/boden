#include <bdn/Switch.h>

namespace bdn
{
    namespace detail
    {
        VIEW_CORE_REGISTRY_IMPLEMENTATION(Switch)
    }

    Switch::Switch(std::shared_ptr<ViewCoreFactory> viewCoreFactory) : View(std::move(viewCoreFactory))
    {
        detail::VIEW_CORE_REGISTER(Switch, View::viewCoreFactory());
        _onClick = std::make_shared<SimpleNotifier<const ClickEvent &>>();
    }

    TriState Switch::state() const { return on ? TriState::on : TriState::off; }

    ISyncNotifier<const ClickEvent &> &Switch::onClick() { return *_onClick; }

    void Switch::bindViewCore()
    {
        View::bindViewCore();
        auto switchCore = core<Switch::Core>();
        switchCore->label.bind(label);
        switchCore->on.bind(on);
    }
}
