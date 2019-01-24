
#include <bdn/View.h>

#include <bdn/LayoutCoordinator.h>
#include <bdn/debug.h>
#include <bdn/ProgrammingError.h>

namespace bdn
{

    View::View()
    {
        visible.onChange() += CorePropertyUpdater<bool, IViewCore>(this, &IViewCore::setVisible);

        size.onChange() += [this](auto) { Influences_(this).influencesContentLayout(); };

        margin.onChange() += CorePropertyUpdater<UiMargin, IViewCore>(this, &IViewCore::setMargin, [](auto &inf) {
            inf.influencesParentPreferredSize().influencesParentLayout();
        });

        padding.onChange() += CorePropertyUpdater<std::optional<UiMargin>, IViewCore>(
            this, &IViewCore::setPadding, [](auto &inf) { inf.influencesPreferredSize().influencesContentLayout(); });

        verticalAlignment.onChange() += CorePropertyUpdater<VerticalAlignment, IViewCore>(
            this, &IViewCore::setVerticalAlignment, [](auto &inf) { inf.influencesParentLayout(); });

        horizontalAlignment.onChange() += CorePropertyUpdater<HorizontalAlignment, IViewCore>(
            this, &IViewCore::setHorizontalAlignment, [](auto &inf) { inf.influencesParentLayout(); });

        preferredSizeHint.onChange() += CorePropertyUpdater<Size, IViewCore>(
            this, &IViewCore::setPreferredSizeHint, [](auto &inf) { inf.influencesPreferredSize(); });

        preferredSizeMinimum.onChange() += CorePropertyUpdater<Size, IViewCore>(
            this, &IViewCore::setPreferredSizeMinimum, [](auto &inf) { inf.influencesPreferredSize(); });

        preferredSizeMaximum.onChange() += CorePropertyUpdater<Size, IViewCore>(
            this, &IViewCore::setPreferredSizeMaximum, [](auto &inf) { inf.influencesPreferredSize(); });

        visible = true; // most views are initially visible
        preferredSizeHint = Size::none();
        preferredSizeMinimum = Size::none();
        preferredSizeMaximum = Size::none();
    }

    View::~View()
    {
        // the core should already be deinitialized at this point (since
        // deleteThis should have run). But we call it again anyway for safety.
        _deinitCore();
    }

    /*
     * TODO
     * void View::deleteThis()
    {
        // Note that deleteThis can be called from ANY thread (we allow view
        // object references to be passed to other threads, even though they are
        // not allowed to access them directly from the other thread).

        // Also note that when this is called, weak pointers to this view
        // already consider the View to be gone. I.e. WeakP::toStrong already
        // returns null, so no new strong references can be created.

        // For views we have the special case that we allow their last reference
        // to be released from any thread, even though the view itself can only
        // be used from the main thread. That allows view objects to be passed
        // to background threads without having to think about who releases the
        // last reference.

        // When a view is destructed then we need to do a lot of things: we need
        // to remove the child views and ensure that their parent pointer is
        // null. We also need to remove / reinit their core. Those are all
        // operations that can only be done from the main thread. So we do not
        // want the destruction to begin in any other thread.
        if (!AppRunnerBase::isMainThread()) {
            // we are not in the main thread. So we need to stop here and
            // continue the destruction in the main thread.

            // note that the lambda function we post here may be executed
            // in the main thread immediately. I.e. even before we exit
            // deleteThis. However, that is OK, since we do not
            // access our object after the asyncCallFromMainThread call. The
            // caller of deleteThis also does not access the object, since
            // deleteThis has the task of deleting the object.

            // Also note that we do not need to increase the reference count
            // when we pass "this" to the main thread, since the object is
            // already on track for deletion and no one else can have or obtain
            // a strong reference to it.

            asyncCallFromMainThread([this]() { deleteThis(); });

            return;
        }

        // We want to detach our child views before we start to destruct
        // ourselves. That ensures that the parent is still fully intact at the
        // time when the child views are destroyed. While the child views cannot
        // access the parent itself anymore at this point (since their weak
        // parent pointers already return null), the child views might still
        // interact with the parent's sub-objects in some way. For example, they
        // might have references to the parent's properties or event notifiers.
        // Note that event notifier references often exist implicitly when an
        // event has been posted but not yet executed. So, to avoid any weird
        // interactions we make sure that the children are fully detached before
        // we start destruction (and before our member objects start to
        // destruct).

        removeAllChildViews();

        // also deinit our core before we start destruction (since the core
        // might still try to access us).
        _deinitCore();

        RequireNewAlloc<Base, View>::deleteThis();
    }
    */

    Rect View::adjustAndSetBounds(const Rect &requestedBounds)
    {
        std::shared_ptr<IViewCore> core = getViewCore();

        Rect adjustedBounds;
        if (core != nullptr) {
            adjustedBounds = core->adjustAndSetBounds(requestedBounds);

            if (!std::isfinite(adjustedBounds.width) || !std::isfinite(adjustedBounds.height) ||
                !std::isfinite(adjustedBounds.x) || !std::isfinite(adjustedBounds.y)) {
                // the preferred size MUST be finite.
                auto &r = *core.get();

                programmingError(String(typeid(r).name()) + ".adjustAndSetBounds returned a non-finite value: " +
                                 std::to_string(adjustedBounds.x) + " , " + std::to_string(adjustedBounds.y) + " " +
                                 std::to_string(adjustedBounds.width) + " x " + std::to_string(adjustedBounds.height));
            }
        } else
            adjustedBounds = requestedBounds;

        // update the position and size properties.
        // Note that the property changes will automatically cause our
        // "modification influence" methods to be called, which will schedule
        // any additional operations that should follow (like re-layout when the
        // size changes, etc.).
        position = adjustedBounds.getPosition();
        size = adjustedBounds.getSize();

        return adjustedBounds;
    }

    Rect View::adjustBounds(const Rect &requestedBounds, RoundType positionRoundType, RoundType sizeRoundType) const
    {
        std::shared_ptr<const IViewCore> core = getViewCore();

        if (core != nullptr) {
            Rect adjustedBounds = core->adjustBounds(requestedBounds, positionRoundType, sizeRoundType);

            if (!std::isfinite(adjustedBounds.width) || !std::isfinite(adjustedBounds.height) ||
                !std::isfinite(adjustedBounds.x) || !std::isfinite(adjustedBounds.y)) {
                // the adjusted bounds MUST be finite.
                auto &r = *core.get();
                programmingError(String(typeid(r).name()) + ".adjustBounds returned a non-finite value: " +
                                 std::to_string(adjustedBounds.x) + " , " + std::to_string(adjustedBounds.y) + " " +
                                 std::to_string(adjustedBounds.width) + " x " + std::to_string(adjustedBounds.height));
            }

            return adjustedBounds;
        } else
            return requestedBounds;
    }

    void View::invalidateSizingInfo(InvalidateReason reason)
    {
        AppRunnerBase::assertInMainThread();

        /*
         * TODO
        if (isBeingDeletedBecauseReferenceCountReachedZero()) {
            // this happens when invalidateSizingInfo is called during the
            // destructor. In this case we do not schedule the invalidation,
            // since the view will be gone anyway.

            // So, do nothing.
            return;
        }*/

        // clear cached sizing data
        _preferredSizeManager.clear();

        // pass the operation to the core. The core will take care
        // of invalidating the layout, if necessary
        std::shared_ptr<IViewCore> core = getViewCore();
        if (core != nullptr)
            core->invalidateSizingInfo(reason);

        std::shared_ptr<View> parentView = getParentView();
        if (parentView != nullptr)
            parentView->childSizingInfoInvalidated(std::dynamic_pointer_cast<View>(shared_from_this()));
    }

    void View::needLayout(InvalidateReason reason)
    {
        AppRunnerBase::assertInMainThread();

        /*
         * TODO
         *  if (isBeingDeletedBecauseReferenceCountReachedZero()) {
            // this happens when invalidateSizingInfo is called during the
            // destructor. In this case we do not schedule the invalidation,
            // since the view will be gone anyway.

            // So, do nothing.
            return;
        }*/

        std::shared_ptr<IViewCore> core = getViewCore();

        // forward the request to the core. Depending on the platform
        // it may be that the UI uses a layout coordinator provided by the
        // system, rather than our own.
        if (core != nullptr)
            core->needLayout(reason);
    }

    double View::uiLengthToDips(const UiLength &length) const
    {
        AppRunnerBase::assertInMainThread();

        if (length.isNone())
            return 0;

        else if (length.unit == UiLength::Unit::dip)
            return length.value;

        else {
            std::shared_ptr<IViewCore> core = getViewCore();

            if (core != nullptr)
                return core->uiLengthToDips(length);
            else
                return 0;
        }
    }

    Margin View::uiMarginToDipMargin(const UiMargin &uiMargin) const
    {
        AppRunnerBase::assertInMainThread();

        std::shared_ptr<IViewCore> core = getViewCore();

        if (core != nullptr)
            return core->uiMarginToDipMargin(uiMargin);
        else
            return Margin();
    }

    void View::childSizingInfoInvalidated(std::shared_ptr<View> child)
    {
        /*
         * TODO
         * if (isBeingDeletedBecauseReferenceCountReachedZero()) {
            // this happens when childSizingInfoInvalidated is called during the
            // destructor. In this case we do not schedule the invalidation,
            // since the view will be gone anyway.

            // So, do nothing.
            return;
        }
        */

        std::shared_ptr<IViewCore> core = getViewCore();

        if (core != nullptr)
            core->childSizingInfoInvalidated(child);
    }

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

        std::shared_ptr<UiProvider> newUiProvider = determineUiProvider(parentView);

        return _uiProvider == newUiProvider && _uiProvider != nullptr && parentView != nullptr && _core != nullptr &&
               _core->canMoveToParentView(parentView);
    }

    void View::reinitCore()
    {
        _deinitCore();

        // at this point our core and the core of our child views is null.

        _initCore();
    }

    void View::_deinitCore()
    {
        std::list<std::shared_ptr<View>> childViewsCopy = getChildViews();

        if (_core != nullptr) {
            _core->dispose();
        }

        _core = nullptr;

        // also release the core of all child views
        for (auto childView : childViewsCopy)
            childView->_deinitCore();
    }

    void View::_initCore()
    {
        // initCore might throw an exception in some cases (for example if the
        // view type is not supported). we want to propagate that exception
        // upwards.

        // If the core is not null then we already have a core. We do nothing in
        // that case.
        if (_core == nullptr) {
            _uiProvider = determineUiProvider();

            if (_uiProvider != nullptr)
                _core = _uiProvider->createViewCore(getCoreTypeName(), shared_from_this());

            std::list<std::shared_ptr<View>> childViewsCopy = getChildViews();

            for (auto childView : childViewsCopy)
                childView->_initCore();

            // our old sizing info is obsolete when the core has changed.
            invalidateSizingInfo(View::InvalidateReason::standardPropertyChanged);
        }
    }

    Size View::calcPreferredSize(const Size &availableSpace) const
    {
        AppRunnerBase::assertInMainThread();

        Size preferredSize;
        if (!_preferredSizeManager.get(availableSpace, preferredSize)) {
            std::shared_ptr<IViewCore> core = getViewCore();

            if (core != nullptr) {
                preferredSize = core->calcPreferredSize(availableSpace);

                if (!std::isfinite(preferredSize.width) || !std::isfinite(preferredSize.height)) {
                    // the preferred size MUST be finite.
                    auto &r = *core.get();
                    programmingError(String(typeid(r).name()) + ".calcPreferredSize returned a non-finite value: " +
                                     std::to_string(preferredSize.width) + " x " +
                                     std::to_string(preferredSize.height));
                }

                _preferredSizeManager.set(availableSpace, preferredSize);
            } else
                preferredSize = Size();
        }

        return preferredSize;
    }
}
