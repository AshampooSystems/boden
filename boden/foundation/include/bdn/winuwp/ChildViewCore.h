#ifndef BDN_WINUWP_ChildViewCore_H_
#define BDN_WINUWP_ChildViewCore_H_

#include <bdn/View.h>

#include <bdn/winuwp/util.h>
#include <bdn/winuwp/IViewCoreParent.h>
#include <bdn/winuwp/UiProvider.h>
#include <bdn/winuwp/IUwpViewCore.h>
#include <bdn/winuwp/UwpLayoutBridge.h>

#include <bdn/Dip.h>

#include <cassert>

namespace bdn
{
    namespace winuwp
    {

        /** Base implementation for Windows Universal view cores that are
           children of another view (see IViewCore). Note that top level windows
           do not derive from this - they provider their own implementation of
           IViewCore.
        */
        class ChildViewCore : public Base,
                              BDN_IMPLEMENTS IViewCore,
                              BDN_IMPLEMENTS IUwpViewCore
        {
          public:
            /** Used internally.*/
            ref class ViewCoreEventForwarder : public Platform::Object
            {
                internal : ViewCoreEventForwarder(ChildViewCore *pParent)
                {
                    _pParentWeak = pParent;
                }

                ChildViewCore *getViewCoreIfAlive() { return _pParentWeak; }

              public:
                void dispose() { _pParentWeak = nullptr; }

                void sizeChanged(Platform::Object ^ pSender,
                                 ::Windows::UI::Xaml::SizeChangedEventArgs ^
                                     pArgs)
                {
                    BDN_WINUWP_TO_PLATFORMEXC_BEGIN

                    ChildViewCore *pViewCore = getViewCoreIfAlive();
                    if (pViewCore != nullptr)
                        pViewCore->_uwpSizeChanged();

                    BDN_WINUWP_TO_PLATFORMEXC_END
                }

              private:
                ChildViewCore *_pParentWeak;
            };

            ChildViewCore(View *pOuterView,
                          ::Windows::UI::Xaml::FrameworkElement ^
                              pFrameworkElement,
                          ViewCoreEventForwarder ^ pEventForwarder)
            {
                BDN_WINUWP_TO_STDEXC_BEGIN;

                _outerViewWeak = pOuterView;
                _pFrameworkElement = pFrameworkElement;

                _measureAvailableSpaceMaximum = Size::none();

                _pEventForwarder = pEventForwarder;

                // when windows updates the size of the content canvas then that
                // means that we have to update our layout.
                _pFrameworkElement->SizeChanged +=
                    ref new ::Windows::UI::Xaml::SizeChangedEventHandler(
                        _pEventForwarder, &ViewCoreEventForwarder::sizeChanged);

                setVisible(pOuterView->visible());
                setPreferredSizeHint(pOuterView->preferredSizeHint());

                _addToParent(pOuterView->getParentView());

                BDN_WINUWP_TO_STDEXC_END;
            }

            ~ChildViewCore() { _pEventForwarder->dispose(); }

            void setVisible(const bool &visible) override
            {
                BDN_WINUWP_TO_STDEXC_BEGIN;

                try {
                    _pFrameworkElement->Visibility =
                        visible ? ::Windows::UI::Xaml::Visibility::Visible
                                : ::Windows::UI::Xaml::Visibility::Collapsed;
                }
                catch (::Platform::DisconnectedException ^ e) {
                    // view was already destroyed. Ignore this.
                }

                BDN_WINUWP_TO_STDEXC_END;
            }

            void setPreferredSizeHint(const Size &hint) override
            {
                // most views do not use the hint. So do nothing by default.
            }

            void setMargin(const UiMargin &margin)
            {
                // this must trigger a layout in the parent view.
                // Note that needLayout(childPropertyChanged) will have already
                // been called on the outer parent by the View object. BUT the
                // parent will ignore that, since it is an update that was
                // triggered by a standard property. But in this case we
                // actually do need the update, so we must cause it explicitly.

                P<View> pOuterView = getOuterViewIfStillAttached();
                if (pOuterView != nullptr) {
                    P<View> pParentView = pOuterView->getParentView();
                    if (pParentView != nullptr) {
                        P<IUwpViewCore> pParentCore =
                            tryCast<IUwpViewCore>(pParentView->getViewCore());
                        if (pParentCore != nullptr) {
                            pParentCore->invalidateMeasure();
                            pParentCore->invalidateArrange();
                        }
                    }
                }
            }

            void setHorizontalAlignment(
                const View::HorizontalAlignment &align) override
            {
                // we must validate our arrange info manually, since we ignore
                // needLayout calls that occur because of standard property
                // changes
                invalidateArrange();
            }

            void
            setVerticalAlignment(const View::VerticalAlignment &align) override
            {
                // we must validate our arrange info manually, since we ignore
                // needLayout calls that occur because of standard property
                // changes
                invalidateArrange();
            }

            void setPreferredSizeMinimum(const Size &limit) override
            {
                // we must validate our measure info manually, since we ignore
                // invalidateSizingInfo calls that occur because of standard
                // property changes
                invalidateMeasure();
            }

            void setPreferredSizeMaximum(const Size &limit) override
            {
                // we must validate our measure info manually, since we ignore
                // invalidateSizingInfo calls that occur because of standard
                // property changes
                invalidateMeasure();
            }

            void invalidateSizingInfo(View::InvalidateReason reason) override
            {
                // see needLayout for an explanation about why we ignore
                // standard property changes.
                if (reason != View::InvalidateReason::standardPropertyChanged &&
                    reason !=
                        View::InvalidateReason::standardChildPropertyChanged) {
                    // OutputDebugString(
                    // (String(typeid(*this).name())+".needSizingInfoUpdate()\n"
                    // ).asWidePtr() );

                    // we leave the layout coordination up to windows. See
                    // doc_input/winuwp_layout.md for more information on why
                    // this is.
                    invalidateMeasure();
                }
            }

            void childSizingInfoInvalidated(View *pChild) override
            {
                // While windows takes care of propagating the
                // "invalidateMeasure" request to parents automatically, we
                // still HAVE to propagate this event upwards to our parent,
                // since the outer view objects cache sizing info and these
                // caches must be invalidated for all parents.
                P<View> pOuterView = getOuterViewIfStillAttached();
                if (pOuterView != nullptr) {
                    pOuterView->invalidateSizingInfo(
                        View::InvalidateReason::childSizingInfoInvalidated);
                    pOuterView->needLayout(
                        View::InvalidateReason::childSizingInfoInvalidated);
                }
            }

            void needLayout(View::InvalidateReason reason) override
            {
                // we ignore layout requests that were made because a standard
                // property changed.

                // That is very important because otherwise we would invalidate
                // the layout when, for example, the View::size() property
                // changes. However, since the size only changes during the
                // layout cycle and windows automatically ensures that the
                // layout of child views is updated that means that we HAVE to
                // ignore such changes -- otherwise we end up scheduling a
                // re-layout in every layout and end up in a cycle.

                // For the standard properties one of our core functions is
                // called and we handle the changes directly there. Usually
                // Windows takes care of the invalidation automatically and if
                // not then the core setXYZ function must schedule the update.

                if (reason != View::InvalidateReason::standardPropertyChanged &&
                    reason !=
                        View::InvalidateReason::standardChildPropertyChanged) {
                    // OutputDebugString(
                    // (String(typeid(*this).name())+".needLayout()\n"
                    // ).asWidePtr() );

                    // a full relayout only happens if another measure cycle is
                    // done (since we actually do our own layout at the end of
                    // measure, not in arrange). So we use invalidateMeasure
                    // instead of invalidateArrange.
                    invalidateMeasure();
                    invalidateArrange();
                }
            }

            Rect adjustAndSetBounds(const Rect &requestedBounds)
            {
                BDN_WINUWP_TO_STDEXC_BEGIN;

                // OutputDebugString(
                // (String(typeid(*this).name())+".adjustAndSetBounds("+std::to_string(requestedBounds.width)+",
                // "+std::to_string(requestedBounds.height)+"\n" ).asWidePtr()
                // );

                // first adjust the bounds
                Rect adjustedBounds = adjustBounds(
                    requestedBounds, RoundType::nearest, RoundType::nearest);

                // Layout is performed at the end of the Measure phase in
                // uwpMeasureFinalize, not in the Arrange phase (see
                // doc_input/winuwp_layout.md for more information). Since
                // adjustAndSetBounds is called from layout, we are currently at
                // the end of the Measure phase. So what we do here is simply
                // store the new bounds. The bounds are made active in the next
                // arrange call.
                _currBounds = adjustedBounds;
                _currBoundsInitialized = true;

                Size assignedSize = adjustedBounds.getSize();

                // we MUST also call Measure here with the final size of the
                // view as available space. This has two effects: 1) it ensures
                // that the UWP element's DesiredSize is smaller or equal to the
                // new view size.
                //    This is important because Windows will not allow us to
                //    make the view smaller than its DesiredSize.
                // 2) We need to ensure that Measure is called on all views that
                // participate in the layout cycle.
                //    Otherwise windows will ignore subsequent Arrange call and
                //    we cannot modify this view.
                ::Windows::UI::Xaml::FrameworkElement ^ pElement =
                    getFrameworkElement();
                if (pElement != nullptr) {
                    // OutputDebugString( (String(typeid(*this).name())+"
                    // Measure("+std::to_string(assignedSize.width)+",
                    // "+std::to_string(assignedSize.height)+"\n" ).asWidePtr()
                    // );

                    ::Windows::Foundation::Size winAssignedSize =
                        sizeToUwpSize(assignedSize);

                    // Windows will actually size the control exactly to the
                    // desired size. We cannot make it bigger if it does not
                    // want it to be. So to force the element to have the size
                    // we want it to have we set the Width and Height
                    // properties.

                    pElement->Width = winAssignedSize.Width;
                    pElement->Height = winAssignedSize.Height;

                    pElement->Measure(winAssignedSize);
                }

                // OutputDebugString(
                // ("/"+String(typeid(*this).name())+".adjustAndSetBounds()\n"
                // ).asWidePtr() );

                return adjustedBounds;

                BDN_WINUWP_TO_STDEXC_END;
            }

            Rect adjustBounds(const Rect &requestedBounds,
                              RoundType positionRoundType,
                              RoundType sizeRoundType) const
            {
                // UWP also uses DIPs and floating point values, so it might
                // seem as if no conversion were necessary. However, UWP also
                // implicitly rounds to full pixels (unless UseLayoutRounding is
                // manually set to false). We want to make this implicit process
                // explicit and do the proper rounding in our code, with our
                // parameters.

                double scaleFactor = UiProvider::get().getUiScaleFactor();

                // the scale factor indicates how many physical pixels there are
                // per DIP. So we want to round to a multiple of that.
                Rect adjustedBounds =
                    Dip::pixelAlign(requestedBounds, scaleFactor,
                                    positionRoundType, sizeRoundType);

                return adjustedBounds;
            }

            double uiLengthToDips(const UiLength &uiLength) const override
            {
                switch (uiLength.unit) {
                case UiLength::Unit::none:
                    return 0;

                case UiLength::Unit::dip:
                    return uiLength.value;

                case UiLength::Unit::em:
                    return uiLength.value * getEmSizeDips();

                case UiLength::Unit::sem:
                    return uiLength.value * getSemSizeDips();

                default:
                    throw InvalidArgumentError(
                        "Invalid UiLength unit passed to "
                        "ViewCore::uiLengthToDips: " +
                        std::to_string((int)uiLength.unit));
                }
            }

            Margin uiMarginToDipMargin(const UiMargin &margin) const override
            {
                return Margin(
                    uiLengthToDips(margin.top), uiLengthToDips(margin.right),
                    uiLengthToDips(margin.bottom), uiLengthToDips(margin.left));
            }

            bool tryChangeParentView(View *pNewParent) override
            {
                _addToParent(pNewParent);

                return true;
            }

            void updateOrderAmongSiblings()
            {
                // we do not care about ordering
            }

            void layout() override
            {
                // do nothing by default
            }

            Size calcPreferredSize(
                const Size &availableSpace = Size::none()) const override
            {
                // OutputDebugString(
                // (String(typeid(*this).name())+".calcPreferredSize("+std::to_string(availableSpace.width)+",
                // "+std::to_string(availableSpace.height)+"\n" ).asWidePtr() );

                BDN_WINUWP_TO_STDEXC_BEGIN;

                // Most views will clip the size returned by Measure to never
                // exceed the specified max width or height (even though Measure
                // is actually documented to return a bigger size if the view
                // cannot be made small enough to fit).

                try {
                    Size effectiveAvailableSpace(availableSpace);

                    // if the control cannot adjust its width to the available
                    // space then
                    // we always report unlimited available space.
                    // The reason is that Windows will otherwise clip the
                    // DesiredSize to the available space. But for the Boden
                    // system we want do not want to clip here - we want to
                    // report the bigger size need to our parent and leave the
                    // decision to clip or not clip to the parent.
                    if (!canAdjustWidthToAvailableSpace())
                        effectiveAvailableSpace.width = Size::componentNone();

                    if (!canAdjustHeightToAvailableSpace())
                        effectiveAvailableSpace.height = Size::componentNone();

                    P<View> pOuter = getOuterViewIfStillAttached();
                    if (pOuter != nullptr) {
                        // preferredSizeMaximum is a hard limit, which is
                        // exactly how most UWP controls interpret the available
                        // size. So we incorporate it.
                        effectiveAvailableSpace.applyMaximum(
                            pOuter->preferredSizeMaximum());
                    }

                    effectiveAvailableSpace.applyMaximum(
                        _measureAvailableSpaceMaximum);

                    ::Windows::Foundation::Size winAvailableSpace =
                        sizeToUwpSize(effectiveAvailableSpace);

                    ::Windows::UI::Xaml::Visibility oldVisibility =
                        _pFrameworkElement->Visibility;
                    if (oldVisibility !=
                        ::Windows::UI::Xaml::Visibility::Visible) {
                        // invisible elements all report a zero size. So we must
                        // make the element temporarily visible
                        _pFrameworkElement->Visibility =
                            ::Windows::UI::Xaml::Visibility::Visible;
                    }

                    // We use the Width and Height property to force the view to
                    // have exactly the size we want it to have. If we leave
                    // them set then Measure will simply return those values
                    // back to us. So we must clear these values.

                    // Note that we do not need to restore the old values after
                    // merasure. See the comments below for an explanation.

                    if (!std::isnan(_pFrameworkElement->Width))
                        _pFrameworkElement->Width =
                            std::numeric_limits<double>::quiet_NaN();
                    if (!std::isnan(_pFrameworkElement->Height))
                        _pFrameworkElement->Height =
                            std::numeric_limits<double>::quiet_NaN();

                    // the Width and Height UIElement properties indicate to the
                    // layout process how big we want to be. If they are set
                    // then they are incorporated into the DesiredSize
                    // measurements. So it sounds like they are analogous to our
                    // preferredSizeHint. However, for many views the "hint
                    // size" is seen as an exact size that Measure will simply
                    // return unchanged. I.e. we will never get anything below
                    // this size if we set it. So we cannot generally use this
                    // as the hint. In the default case we do not pass the hint
                    // on to the control. The specific subclasses of view need
                    // to implement the hinting for their specific case with the
                    // knowledge what the specific control will do with this
                    // information.

                    _pFrameworkElement->Measure(winAvailableSpace);
                    Size preferredSize =
                        uwpSizeToSize(_pFrameworkElement->DesiredSize);

                    // Windows does not allow UIElements to be smaller than
                    // their DesiredSize.

                    // if we are currently in a Measure cycle then our parent
                    // will make sure that we end up with the correct
                    // DesiredSize at the end of the Measure phase.

                    // If we are not in a measure cycle (i.e. if the app called
                    // calcPreferredSize manually, then we now have a
                    // DesiredSize that may not fit our assigned size. We could
                    // call Measure again here and ensure that DesiredSize
                    // reverts back to the old value. However, DesiredSize has
                    // no effect outside the layout cycle, so it does not hurt
                    // to leave it at the current value. And when the next
                    // layout cycle starts then our layout implementations will
                    // make sure that DesiredSize is set correctly.

                    // So there is no need to revert to the old DesiredSize
                    // here.

                    // For the same reason we do not need to reset the Width and
                    // Height properties to their previous values.

                    // But we do have to restore the old visibility information.
                    if (oldVisibility !=
                        ::Windows::UI::Xaml::Visibility::Visible)
                        _pFrameworkElement->Visibility = oldVisibility;

                    if (pOuter != nullptr) {
                        preferredSize.applyMinimum(
                            pOuter->preferredSizeMinimum());

                        // clip to the maximum again. We never want that to be
                        // exceeded, even if the content does not fit.
                        preferredSize.applyMaximum(
                            pOuter->preferredSizeMaximum());
                    }

                    // OutputDebugString(
                    // ("/"+String(typeid(*this).name())+".calcPreferredSize()
                    // -> desiredSize= "+std::to_string(preferredSize.width)+",
                    // "+std::to_string(preferredSize.height)+"\n" ).asWidePtr()
                    // );

                    return preferredSize;
                }
                catch (::Platform::DisconnectedException ^ e) {
                    // view was already destroyed. Ignore this and return zero
                    // size
                    return Size();
                }

                BDN_WINUWP_TO_STDEXC_END;
            }

            /** Returns the XAML FrameworkElement object for this view.*/
            ::Windows::UI::Xaml::FrameworkElement ^
                getFrameworkElement() override
            {
                return _pFrameworkElement;
            }

            /** Returns a pointer to the outer view object that is associated
               with this core. Can return null if the core has been disposed
               (i.e. if it is not connected to an outer view anymore).*/
            P<View> getOuterViewIfStillAttached() const
            {
                return _outerViewWeak.toStrong();
            }

          protected:
            /** Returns true if the view can adjust its width to fit into
                a certain size of available space.

                If this returns false then calcPreferredSize will ignore the
                availableWidth parameter.

                The default implementation returns false.
            */
            virtual bool canAdjustWidthToAvailableSpace() const
            {
                return false;
            }

            /** Returns true if the view can adjust its height to fit into
                a certain size of available space.

                If this returns false then calcPreferredSize will ignore the
                availableHeight parameter.

                The default implementation returns false.
            */
            virtual bool canAdjustHeightToAvailableSpace() const
            {
                return false;
            }

            ViewCoreEventForwarder ^
                getViewCoreEventForwarder() { return _pEventForwarder; }

                virtual void _uwpSizeChanged()
            {
                // nothing to do here. Windows takes care of updating our
                // layout.
            }

            double getEmSizeDips() const
            {
                if (_emSizeDipsIfInitialized == -1) {
                    // FrameworkElement does not have a FontSize property. Only
                    // Control and TextBlock objects have a font size.

                    ::Windows::UI::Xaml::Controls::Control ^ pControl =
                        dynamic_cast<::Windows::UI::Xaml::Controls::Control ^>(
                            _pFrameworkElement);
                    if (pControl != nullptr)
                        _emSizeDipsIfInitialized = pControl->FontSize;
                    else {
                        ::Windows::UI::Xaml::Controls::TextBlock ^ pTextBlock =
                            dynamic_cast<
                                ::Windows::UI::Xaml::Controls::TextBlock ^>(
                                _pFrameworkElement);
                        if (pTextBlock != nullptr)
                            _emSizeDipsIfInitialized = pTextBlock->FontSize;
                        else {
                            // use the default font size (which is documented as
                            // being 11 DIPs).
                            _emSizeDipsIfInitialized = 11;
                        }
                    }
                }

                return _emSizeDipsIfInitialized;
            }

            double getSemSizeDips() const
            {
                if (_semSizeDipsIfInitialized == -1)
                    _semSizeDipsIfInitialized =
                        UiProvider::get().getSemSizeDips();

                return _semSizeDipsIfInitialized;
            }

          protected:
            /** Sets an artificial upper limit for the available space that is
               communicated to the control in calcPreferredSize.

                This can be used, for example, to force the control to wrap its
               text content at a certain point.

                Each component of the limit size can be infinity to indicate
               that there should be no artificial limit.
                */
            void setMeasureAvailableSpaceMaximum(const Size &limit)
            {
                _measureAvailableSpaceMaximum = limit;
            }

            void invalidateMeasure() override
            {
                UwpLayoutBridge::get().invalidateMeasure(_pFrameworkElement);
            }

            void invalidateArrange() override
            {
                UwpLayoutBridge::get().invalidateArrange(_pFrameworkElement);
            }

          private:
            void _addToParent(View *pParentView)
            {
                if (pParentView == nullptr) {
                    // classes derived from ViewCore MUST have a parent. Top
                    // level windows do not derive from ViewCore.
                    throw ProgrammingError(
                        "bdn::winuwp::ViewCore constructed for a view that "
                        "does not have a parent.");
                }

                P<IViewCore> pParentCore = pParentView->getViewCore();
                if (pParentCore == nullptr) {
                    // this should not happen. The parent MUST have a core -
                    // otherwise we cannot initialize ourselves.
                    throw ProgrammingError(
                        "bdn::winuwp::ViewCore constructed for a view whose "
                        "parent does not have a core.");
                }

                cast<IViewCoreParent>(pParentCore)
                    ->addChildUiElement(_pFrameworkElement);
            }

            ::Windows::UI::Xaml::FrameworkElement ^ _pFrameworkElement;

            WeakP<View> _outerViewWeak; // weak by design

            ViewCoreEventForwarder ^ _pEventForwarder;

            mutable double _emSizeDipsIfInitialized = -1;
            mutable double _semSizeDipsIfInitialized = -1;

            bool _currBoundsInitialized = false;
            Rect _currBounds;

            Size _measureAvailableSpaceMaximum;
        };
    }
}

#endif
