
#include <bdn/ui/Checkbox.h>

#include <utility>

namespace bdn::ui
{
    namespace detail
    {
        VIEW_CORE_REGISTRY_IMPLEMENTATION(Checkbox)
    }

    Checkbox::Checkbox(std::shared_ptr<ViewCoreFactory> viewCoreFactory) : View(std::move(viewCoreFactory))
    {
        detail::VIEW_CORE_REGISTER(Checkbox, View::viewCoreFactory());
    }

    Notifier<const ClickEvent &> &Checkbox::onClick() { return _onClick; }

    void Checkbox::bindViewCore()
    {
        View::bindViewCore();
        auto checkBoxCore = core<Checkbox::Core>();
        checkBoxCore->label.bind(label);
        checkBoxCore->state.bind(state);

        _clickCallbackReceiver = checkBoxCore->_clickCallback.set([=]() {
            ClickEvent evt(shared_from_this());
            onClick().notify(evt);
        });
    }
}
