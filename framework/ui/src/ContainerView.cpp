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
            childView->setParentView(shared_from_this());
        }
    }

    void ContainerView::removeChildView(const std::shared_ptr<View> &childView)
    {
        if (auto containerCore = core<ContainerView::Core>()) {
            containerCore->removeChildView(childView);
            childView->setParentView(nullptr);
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

    std::list<std::shared_ptr<View>> ContainerView::childViews()
    {
        if (auto containerCore = core<ContainerView::Core>()) {
            return containerCore->childViews();
        }

        throw std::runtime_error("???");
    }

    void ContainerView::childViewStolen(const std::shared_ptr<View> &childView)
    {
        if (auto containerCore = core<ContainerView::Core>()) {
            containerCore->removeChildView(childView);
        } else {
            throw std::runtime_error("???");
        }
    }
}
