#include <bdn/ui/ContainerView.h>

namespace bdn::ui
{
    namespace detail
    {
        VIEW_CORE_REGISTRY_IMPLEMENTATION(ContainerView)
    }

    ContainerView::ContainerView(std::shared_ptr<ViewCoreFactory> viewCoreFactory) : View(std::move(viewCoreFactory))
    {
        detail::VIEW_CORE_REGISTER(ContainerView, View::viewCoreFactory());
    }

    void ContainerView::addChildView(const std::shared_ptr<View> &childView)
    {
        if (auto containerCore = core<ContainerView::Core>()) {
            containerCore->addChildView(childView);
            View::setParentViewOfView(childView, shared_from_this());
        }
    }

    void ContainerView::removeChildView(const std::shared_ptr<View> &childView)
    {
        if (auto containerCore = core<ContainerView::Core>()) {
            containerCore->removeChildView(childView);
            View::setParentViewOfView(childView, nullptr);
        } else {
            throw std::runtime_error("???");
        }
    }

    void ContainerView::removeAllChildViews()
    {
        auto copyChildren = childViews();

        for (auto &childView : copyChildren) {
            removeChildView(childView);
        }
    }

    std::vector<std::shared_ptr<View>> ContainerView::childViews() const
    {
        if (auto containerCore = core<ContainerView::Core>()) {
            return containerCore->childViews();
        }

        throw std::runtime_error("???");
    }
}
