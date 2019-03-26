
#include <bdn/TextField.h>

namespace bdn
{
    namespace detail
    {
        VIEW_CORE_REGISTRY_IMPLEMENTATION(TextField)
    }

    TextField::TextField(std::shared_ptr<ViewCoreFactory> viewCoreFactory) : View(std::move(viewCoreFactory))
    {
        detail::VIEW_CORE_REGISTER(TextField, View::viewCoreFactory());
        _onSubmit = std::make_shared<SimpleNotifier<const SubmitEvent &>>();
    }

    void TextField::submit()
    {
        bdn::SubmitEvent event(shared_from_this());
        onSubmit().notify(event);
    }

    ISyncNotifier<const SubmitEvent &> &TextField::onSubmit() { return *_onSubmit; }

    void TextField::bindViewCore()
    {
        View::bindViewCore();
        auto textCore = View::core<TextFieldCore>();
        textCore->text.bind(text);

        _submitCallbackReceiver = textCore->submitCallback.set([=]() {
            SubmitEvent evt(shared_from_this());
            onSubmit().notify(evt);
        });
    }
}
