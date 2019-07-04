#include <bdn/ui/TextField.h>
#include <bdn/ui/ViewUtilities.h>

namespace bdn::ui
{
    namespace detail
    {
        VIEW_CORE_REGISTRY_IMPLEMENTATION(TextField)
    }

    TextField::TextField(std::shared_ptr<ViewCoreFactory> viewCoreFactory) : View(std::move(viewCoreFactory))
    {
        detail::VIEW_CORE_REGISTER(TextField, View::viewCoreFactory());
    }

    void TextField::submit()
    {
        SubmitEvent event(shared_from_this());
        onSubmit().notify(event);
    }

    Notifier<const SubmitEvent &> &TextField::onSubmit() { return _onSubmit; }

    void TextField::bindViewCore()
    {
        View::bindViewCore();
        auto textCore = View::core<TextField::Core>();
        textCore->text.bind(text);
        textCore->font.bind(font);

        _submitCallbackReceiver = textCore->submitCallback.set([=]() {
            SubmitEvent evt(shared_from_this());
            onSubmit().notify(evt);
        });
    }

    void TextField::updateFromStylesheet()
    {
        if (stylesheet->count("font")) {
            Font f = stylesheet->at("font");
            font = f;
        }

        if (stylesheet->count("text")) {
            text = stylesheet->at("text");
        }

        View::updateFromStylesheet();
    }
}
