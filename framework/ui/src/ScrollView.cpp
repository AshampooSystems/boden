
#include <bdn/ScrollView.h>

#include <bdn/Application.h>

namespace bdn
{
    namespace detail
    {
        VIEW_CORE_REGISTRY_IMPLEMENTATION(ScrollView)
    }

    ScrollView::ScrollView(std::shared_ptr<ViewCoreFactory> viewCoreFactory)
        : View(std::move(viewCoreFactory)), verticalScrollingEnabled(true)
    {
        detail::VIEW_CORE_REGISTER(ScrollView, View::viewCoreFactory());
        contentView.onChange() += [=](auto va) { _contentView.update(shared_from_this(), va->get()); };
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

    std::list<std::shared_ptr<View>> ScrollView::childViews()
    {
        assert(Application::isMainThread());

        if (contentView.get() != nullptr) {
            return {contentView.get()};
        }

        return {};
    }

    void ScrollView::childViewStolen(const std::shared_ptr<View> &childView)
    {
        assert(Application::isMainThread());

        if (childView == contentView.get()) {
            contentView = nullptr;
        }
    }
}
