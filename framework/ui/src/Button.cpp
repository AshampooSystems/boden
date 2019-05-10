#include <bdn/ui/Button.h>

namespace bdn::ui
{
    namespace detail
    {
        VIEW_CORE_REGISTRY_IMPLEMENTATION(Button)
    }

    Button::Button(std::shared_ptr<ViewCoreFactory> viewCoreFactory) : View(std::move(viewCoreFactory))
    {
        detail::VIEW_CORE_REGISTER(Button, View::viewCoreFactory());
    }

    Notifier<const ClickEvent &> &Button::onClick() { return _onClick; }

    void Button::bindViewCore()
    {
        View::bindViewCore();
        auto buttonCore = core<Button::Core>();
        buttonCore->label.bind(label);
        _clickCallbackReceiver = buttonCore->_clickCallback.set([=]() {
            ClickEvent evt(shared_from_this());
            _onClick.notify(evt);
        });
    }
}
