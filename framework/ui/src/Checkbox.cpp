
#include <bdn/Checkbox.h>

#include <utility>

namespace bdn
{
    namespace detail
    {
        VIEW_CORE_REGISTRY_IMPLEMENTATION(Checkbox)
    }

    Checkbox::Checkbox(std::shared_ptr<ViewCoreFactory> viewCoreFactory) : View(std::move(viewCoreFactory))
    {
        detail::VIEW_CORE_REGISTER(Checkbox, View::viewCoreFactory());
        _onClick = std::make_shared<SimpleNotifier<const ClickEvent &>>();
    }

    ISyncNotifier<const ClickEvent &> &Checkbox::onClick() { return *_onClick; }

    void Checkbox::bindViewCore()
    {
        View::bindViewCore();
        auto checkBoxCore = core<CheckboxCore>();
        checkBoxCore->label.bind(label);
        checkBoxCore->state.bind(state);

        _clickCallbackReceiver = checkBoxCore->_clickCallback.set([=]() {
            ClickEvent evt(shared_from_this());

            onClick().notify(evt);
        });
    }
}
