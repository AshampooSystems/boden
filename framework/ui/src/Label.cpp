
#include <bdn/ui/Label.h>
#include <bdn/ui/Text.h>

namespace bdn::ui
{
    namespace detail
    {
        VIEW_CORE_REGISTRY_IMPLEMENTATION(Label)
    }

    Label::Label(std::shared_ptr<ViewCoreFactory> viewCoreFactory) : View(std::move(viewCoreFactory))
    {
        detail::VIEW_CORE_REGISTER(Label, View::viewCoreFactory());
    }

    Notifier<const std::string &> &Label::onLinkClick() { return _onLinkClick; }

    void Label::bindViewCore()
    {
        View::bindViewCore();
        auto textCore = View::core<Label::Core>();
        textCore->text.bind(text);
        textCore->wrap.bind(wrap);
        textCore->textOverflow.bind(textOverflow);

        _linkClickCallbackReceiver =
            textCore->_linkClickCallback.set([=](const auto &link) { _onLinkClick.notify(link); });
    }

    void Label::updateFromStylesheet() { View::updateFromStylesheet(); }
}
