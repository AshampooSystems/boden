#include <bdn/View.h>

#include <bdn/ProgrammingError.h>
#include <bdn/UIProvider.h>
#include <bdn/debug.h>

namespace bdn
{

    View::View() : _hasLayoutSchedulePending(false)
    {
        visible = true; // most views are initially visible
        layoutStylesheet.onChange() += [=](auto va) {
            if (auto layout = _layout.get()) {
                layout->updateStylesheet(this);
            }
        };
    }

    View::~View()
    {
        _deinitCore();
        setLayout(nullptr);
    }

    void View::setLayout(std::shared_ptr<Layout> layout)
    {
        auto oldLayout = _layout.set(layout);
        updateLayout(oldLayout, _layout.get());
    }

    void View::offerLayout(std::shared_ptr<Layout> layout)
    {
        auto oldLayout = _layout.setOffered(layout);

        updateLayout(oldLayout, _layout.get());
    }

    void View::updateLayout(std::shared_ptr<Layout> oldLayout, std::shared_ptr<Layout> newLayout)
    {
        if (oldLayout) {
            oldLayout->unregisterView(this);
        }

        if (newLayout) {
            newLayout->registerView(this);
        }

        for (auto child : getChildViews()) {
            child->offerLayout(newLayout);
        }
    }

    std::shared_ptr<Layout> View::getLayout() { return _layout.get(); }

    void View::_setParentView(std::shared_ptr<View> parentView)
    {
        AppRunnerBase::assertInMainThread();

        if (_canMoveToParentView(parentView)) {
            // Move the core directly to its new parent without reinitialization
            _core->moveToParentView(parentView);

            _parentViewWeak = parentView;
        } else {
            _deinitCore();

            _parentViewWeak = parentView;

            // Note that there is no need to update the UI provider of the child
            // views. If the UI provider changed then the core will be
            // reinitialized. That automatically causes our child cores to be
            // reinitialized which in turn updates their UI provider.

            if (parentView)
                reinitCore();
        }

        if (parentView) {
            offerLayout(parentView->getLayout());
        } else {
            offerLayout(nullptr);
        }
    }

    bool View::_canMoveToParentView(std::shared_ptr<View> parentView)
    {
        // Note that we do not have special handling for the case when the "new"
        // parent view is the same as the old parent view. That case can happen
        // if the order position of a child view inside the parent is changed.
        // We use the same handling for that as for the case of a different
        // handling: ask the core to update accordingly. And the core gets the
        // opportunity to deny that, causing us to recreate the core (maybe in
        // some cases the core cannot change the order of existing views).
        //
        // See if we need to throw away our current core and create a new one.
        // The reason why we don't always throw this away is that the change in
        // parents might simply be a minor layout position change, and in that
        // case the UI provider might want to animate this change. To allow for
        // that, we have to keep the old core and tell it to switch parents.
        // Note that we can only keep the current core if the old and new
        // parent's use the same UI provider.

        std::shared_ptr<UIProvider> newUIProvider = determineUIProvider(parentView);

        return _uiProvider == newUIProvider && _uiProvider != nullptr && parentView != nullptr && _core != nullptr &&
               _core->canMoveToParentView(parentView);
    }

    void View::reinitCore(std::shared_ptr<UIProvider> uiProvider)
    {
        _deinitCore();
        _initCore(uiProvider);
    }

    void View::scheduleLayout()
    {
        if (auto viewCore = getViewCore()) {
            viewCore->scheduleLayout();
        } else {
            _hasLayoutSchedulePending = true;
        }
    }

    void View::bindViewCore()
    {
        if (auto viewCore = getViewCore()) {
            viewCore->visible.bind(visible);
            viewCore->geometry.bind(geometry);
        }
    }

    void View::_deinitCore() { setViewCore(nullptr, nullptr); }

    void View::_initCore(std::shared_ptr<UIProvider> uiProvider)
    {
        uiProvider = uiProvider ? uiProvider : determineUIProvider();
        if (_core == nullptr) {
            if (uiProvider != nullptr) {
                setViewCore(uiProvider, uiProvider->createViewCore(getCoreTypeName(), shared_from_this()));
            }
        }
    }

    void View::setViewCore(std::shared_ptr<UIProvider> uiProvider, std::shared_ptr<ViewCore> viewCore)
    {
        if (_core && _core != viewCore) {
            _core->dispose();
        }

        if (viewCore) {
            _core = std::move(viewCore);
            _uiProvider = uiProvider;

            bindViewCore();

            std::list<std::shared_ptr<View>> childViewsCopy = getChildViews();
            for (auto childView : childViewsCopy) {
                childView->_setParentView(_core ? shared_from_this() : nullptr);
            }

            if (_hasLayoutSchedulePending) {
                _core->scheduleLayout();
                _hasLayoutSchedulePending = false;
            }

        } else {

            std::list<std::shared_ptr<View>> childViewsCopy = getChildViews();
            for (auto childView : childViewsCopy) {
                childView->_setParentView(viewCore ? shared_from_this() : nullptr);
            }

            _core = std::move(viewCore);
            _uiProvider = uiProvider;
        }
    }

    Size View::sizeForSpace(Size availableSpace) const
    {
        if (auto viewCore = this->getViewCore()) {
            return viewCore->sizeForSpace(availableSpace);
        }
        return Size{0, 0};
    }
}
