
#include <bdn/Text.h>
#include <bdn/ui/Label.h>

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
        textCore->truncateMode.bind(truncateMode);

        _linkClickCallbackReceiver =
            textCore->_linkClickCallback.set([=](const auto &link) { _onLinkClick.notify(link); });
    }

    void Label::updateFromStylesheet()
    {
        View::updateFromStylesheet();

        if (stylesheet->count("text")) {
            text = stylesheet->at("text").get<bdn::Text>();
        }
    }
}
