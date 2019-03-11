
#include <bdn/ContainerView.h>
#include <bdn/ContainerViewCore.h>

namespace bdn
{

    ContainerView::ContainerView(std::shared_ptr<UIProvider> uiProvider) : View(std::move(uiProvider)) {}

    void ContainerView::addChildView(std::shared_ptr<View> childView)
    {
        if (auto containerCore = core<ContainerViewCore>()) {
            containerCore->addChildView(childView);
            childView->setParentView(shared_from_this());
        }
    }

    void ContainerView::removeChildView(std::shared_ptr<View> childView)
    {
        if (auto containerCore = core<ContainerViewCore>()) {
            containerCore->removeChildView(childView);
            childView->setParentView(nullptr);
        } else {
            throw std::runtime_error("???");
        }
    }

    void ContainerView::removeAllChildViews()
    {
        auto copyChildren = childViews();

        for (auto &childView : copyChildren)
            removeChildView(childView);
    }

    std::list<std::shared_ptr<View>> ContainerView::childViews()
    {
        if (auto containerCore = core<ContainerViewCore>()) {
            return containerCore->childViews();
        }

        throw std::runtime_error("???");
    }

    void ContainerView::childViewStolen(std::shared_ptr<View> childView)
    {
        if (auto containerCore = core<ContainerViewCore>()) {
            containerCore->removeChildView(childView);
        } else {
            throw std::runtime_error("???");
        }
    }
}
