
#include <bdn/Checkbox.h>

namespace bdn
{

    Checkbox::Checkbox(std::shared_ptr<UIProvider> uiProvider) : View(uiProvider)
    {
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
