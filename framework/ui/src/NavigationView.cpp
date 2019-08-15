#include <bdn/ui/NavigationView.h>

#include <utility>

namespace bdn::ui
{
    namespace detail
    {
        VIEW_CORE_REGISTRY_IMPLEMENTATION(NavigationView)
    }

    NavigationView::NavigationView(std::shared_ptr<ViewCoreFactory> viewCoreFactory) : View(std::move(viewCoreFactory))
    {
        detail::VIEW_CORE_REGISTER(NavigationView, View::viewCoreFactory());
    }

    void NavigationView::pushView(std::shared_ptr<View> view, std::string title)
    {
        if (auto navigationViewCore = core<NavigationView::Core>()) {
            navigationViewCore->pushView(std::move(view), std::move(title));
        }
    }

    void NavigationView::popView()
    {
        if (auto navigationViewCore = core<NavigationView::Core>()) {
            navigationViewCore->popView();
        }
    }

    std::vector<std::shared_ptr<View>> NavigationView::childViews() const
    {
        if (auto navigationViewCore = core<NavigationView::Core>()) {
            return navigationViewCore->childViews();
        }
        return {};
    }

    void NavigationView::bindViewCore() { View::bindViewCore(); }
}
