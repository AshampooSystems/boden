
#include <bdn/ScrollView.h>

#include <bdn/AppRunnerBase.h>
#include <bdn/IScrollViewCore.h>

namespace bdn
{

    ScrollView::ScrollView() : verticalScrollingEnabled(true) {}

    void ScrollView::setContentView(std::shared_ptr<View> contentView)
    {
        AppRunnerBase::assertInMainThread();

        horizontalScrollingEnabled.onChange() +=
            CorePropertyUpdater<bool, IScrollViewCore>(this, &IScrollViewCore::setHorizontalScrollingEnabled);

        verticalScrollingEnabled.onChange() +=
            CorePropertyUpdater<bool, IScrollViewCore>(this, &IScrollViewCore::setVerticalScrollingEnabled);

        if (contentView != _contentView) {
            if (_contentView != nullptr)
                _contentView->_setParentView(nullptr);

            _contentView = contentView;

            if (_contentView != nullptr)
                _contentView->_setParentView(shared_from_this());
        }
    }

    std::shared_ptr<View> ScrollView::getContentView()
    {
        AppRunnerBase::assertInMainThread();

        return _contentView;
    }

    std::shared_ptr<const View> ScrollView::getContentView() const
    {
        AppRunnerBase::assertInMainThread();

        return _contentView;
    }

    void ScrollView::scrollClientRectToVisible(const Rect &area)
    {
        AppRunnerBase::assertInMainThread();

        std::shared_ptr<IScrollViewCore> core = std::dynamic_pointer_cast<IScrollViewCore>(getViewCore());
        if (core != nullptr)
            core->scrollClientRectToVisible(area);
    }

    std::list<std::shared_ptr<View>> ScrollView::getChildViews() const
    {
        AppRunnerBase::assertInMainThread();

        if (_contentView != nullptr) {
            return {_contentView};
        }

        return {};
    }

    std::shared_ptr<View> ScrollView::findPreviousChildView(std::shared_ptr<View> childView)
    {
        // we do not have multiple child views with an order - just a single
        // content view
        return nullptr;
    }

    void ScrollView::_childViewStolen(std::shared_ptr<View> childView)
    {
        AppRunnerBase::assertInMainThread();

        if (childView == _contentView)
            _contentView = nullptr;
    }
}
