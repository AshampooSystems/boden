
#include <bdn/ui/ScrollView.h>

#include <bdn/Application.h>

namespace bdn::ui
{
    namespace detail
    {
        VIEW_CORE_REGISTRY_IMPLEMENTATION(ScrollView)
    }

    ScrollView::ScrollView(std::shared_ptr<ViewCoreFactory> viewCoreFactory)
        : View(std::move(viewCoreFactory)), verticalScrollingEnabled(true)
    {
        detail::VIEW_CORE_REGISTER(ScrollView, View::viewCoreFactory());
        contentView.onChange() += [=](auto &property) { _contentView.update(shared_from_this(), property.get()); };
    }

    void ScrollView::bindViewCore()
    {
        View::bindViewCore();

        auto scrollCore = core<ScrollView::Core>();
        scrollCore->contentView.bind(contentView);
        scrollCore->horizontalScrollingEnabled.bind(horizontalScrollingEnabled);
        scrollCore->verticalScrollingEnabled.bind(verticalScrollingEnabled);
    }

    void ScrollView::scrollClientRectToVisible(const Rect &area)
    {
        assert(Application::isMainThread());

        auto scrollCore = core<ScrollView::Core>();
        scrollCore->scrollClientRectToVisible(area);
    }

    std::vector<std::shared_ptr<View>> ScrollView::childViews() const
    {
        assert(Application::isMainThread());

        if (contentView.get() != nullptr) {
            return {contentView.get()};
        }

        return {};
    }
}
