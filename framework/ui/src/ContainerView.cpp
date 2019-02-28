
#include <bdn/ContainerView.h>

namespace bdn
{

    void ContainerView::addChildView(std::shared_ptr<View> childView) { insertChildView(nullptr, childView); }

    void ContainerView::insertChildView(std::shared_ptr<View> insertBeforeChildView, std::shared_ptr<View> childView)
    {
        AppRunnerBase::assertInMainThread();

        std::shared_ptr<View> oldParentView = childView->getParentView();
        if (oldParentView != nullptr) {
            // do not use removeChildView on the old parent. Instead we use
            // childViewStolen. The difference is that with childViewStolen
            // the old parent will NOT call setParentView on its old child.
            // That is what we want since we want a single call to
            // setParentView at the end of the whole operation, so that the
            // core can potentially be moved directly to its new parent,
            // without being destroyed and recreated.
            oldParentView->_childViewStolen(childView);
        }

        std::list<std::shared_ptr<View>>::iterator it;
        if (insertBeforeChildView == nullptr)
            it = _childViews.end();
        else
            it = std::find(std::begin(_childViews), std::end(_childViews), insertBeforeChildView);

        _childViews.insert(it, childView);

        childView->_setParentView(std::static_pointer_cast<View>(shared_from_this()));

        // the child will schedule a sizing info update for us when it gets
        // its core.
    }

    void ContainerView::removeChildView(std::shared_ptr<View> childView)
    {
        AppRunnerBase::assertInMainThread();

        auto it = std::find(_childViews.begin(), _childViews.end(), childView);
        if (it != _childViews.end()) {
            _childViews.erase(it);
            childView->_setParentView(nullptr);
        }
    }

    void ContainerView::removeAllChildViews()
    {
        AppRunnerBase::assertInMainThread();

        auto copyChildren = _childViews;
        _childViews.clear();

        for (auto &childView : copyChildren)
            childView->_setParentView(nullptr);
    }

    std::list<std::shared_ptr<View>> ContainerView::getChildViews() const
    {
        AppRunnerBase::assertInMainThread();

        return _childViews;
    }

    std::shared_ptr<View> ContainerView::findPreviousChildView(std::shared_ptr<View> childView)
    {
        AppRunnerBase::assertInMainThread();

        std::shared_ptr<View> prevChildView;
        for (const std::shared_ptr<View> &currView : _childViews) {
            if (currView == childView)
                return prevChildView;

            prevChildView = currView;
        }

        return nullptr;
    }

    void ContainerView::_childViewStolen(std::shared_ptr<View> childView)
    {
        AppRunnerBase::assertInMainThread();

        auto it = std::find(_childViews.begin(), _childViews.end(), childView);
        if (it != _childViews.end())
            _childViews.erase(it);
    }
}
