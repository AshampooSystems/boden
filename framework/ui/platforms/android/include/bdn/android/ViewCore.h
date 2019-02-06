#pragma once

namespace bdn
{
    namespace android
    {

        class ViewCore;
    }
}

#include <bdn/IViewCore.h>
#include <bdn/Dip.h>
#include <bdn/LayoutCoordinator.h>

#include <bdn/java/NativeWeakPointer.h>

#include <bdn/android/JView.h>
#include <bdn/android/JNativeViewGroup.h>
#include <bdn/android/UIProvider.h>
#include <bdn/android/IParentViewCore.h>

#include <bdn/log.h>

using namespace std::string_literals;

namespace bdn
{
    namespace android
    {

        class ViewCore : public Base, virtual public IViewCore, virtual public LayoutCoordinator::IViewCoreExtension
        {
          public:
            ViewCore(std::shared_ptr<View> outerView, JView jView) : _outerViewWeak(outerView), _jView(jView)
            {
                _uiScaleFactor = 1; // will be updated in _addToParent

                setVisible(outerView->visible);

                _addToParent(outerView->getParentView());

                _defaultPixelPadding = Margin(_jView.getPaddingTop(), _jView.getPaddingRight(),
                                              _jView.getPaddingBottom(), _jView.getPaddingLeft());

                setPadding(outerView->padding);
            }

            ~ViewCore()
            {
                // remove the the reference to ourselves from the java-side
                // view object. Note that we hold a strong reference to the
                // java-side object, So we know that the reference to the
                // java-side object is still valid.
                _jView.setTag(bdn::java::JObject(bdn::java::Reference()));
            }

          public:
            template <class T> static JView createAndroidViewClass(std::shared_ptr<View> outerView)
            {

                std::shared_ptr<View> parent = outerView->getParentView();
                if (parent == nullptr) {
                    throw ProgrammingError("ViewCore instance requested for a "s + typeid(T).name() +
                                           " that does not have a parent."s);
                }

                std::shared_ptr<ViewCore> parentCore = std::dynamic_pointer_cast<ViewCore>(parent->getViewCore());

                if (parentCore == nullptr) {
                    throw ProgrammingError("ViewCore instance requested for a "s + typeid(T).name() +
                                           " with core-less parent."s);
                }

                T view(parentCore->getJView().getContext());

                return JView(view.getRef_());
            }

            static std::shared_ptr<ViewCore> getViewCoreFromJavaViewRef(const bdn::java::Reference &javaViewRef)
            {
                if (javaViewRef.isNull())
                    return nullptr;
                else {
                    JView view(javaViewRef);
                    bdn::java::NativeWeakPointer viewTag(view.getTag().getRef_());

                    return std::static_pointer_cast<ViewCore>(viewTag.getPointer().lock());
                }
            }

            /** Returns a pointer to the outer View object, if this core is
               still attached to it or null otherwise.*/
            std::shared_ptr<View> getOuterViewIfStillAttached() const { return _outerViewWeak.lock(); }

            JView &getJView() { return _jView; }

            template <class JSuperType> JSuperType getJViewAS() { return _jView.cast<JSuperType>(); }

            void setVisible(const bool &visible) override
            {
                _jView.setVisibility(visible ? JView::Visibility::visible : JView::Visibility::invisible);
            }

            void setPadding(const std::optional<UIMargin> &padding) override
            {
                Margin pixelPadding;
                if (!padding)
                    pixelPadding = _defaultPixelPadding;
                else {
                    Margin dipPadding = uiMarginToDipMargin(*padding);

                    pixelPadding = Margin(dipPadding.top * _uiScaleFactor, dipPadding.right * _uiScaleFactor,
                                          dipPadding.bottom * _uiScaleFactor, dipPadding.left * _uiScaleFactor);
                }

                _jView.setPadding(pixelPadding.left, pixelPadding.top, pixelPadding.right, pixelPadding.bottom);
            }

            void setMargin(const UIMargin &margin) override
            {
                // nothing to do. Our parent handles this.
            }

            void invalidateSizingInfo(View::InvalidateReason reason) override
            {
                // nothing to do since we do not cache sizing info in the core.
            }

            void needLayout(View::InvalidateReason reason) override;

            void childSizingInfoInvalidated(std::shared_ptr<View> child) override;

            void setHorizontalAlignment(const View::HorizontalAlignment &align) override
            {
                // do nothing. The View handles this.
            }

            void setVerticalAlignment(const View::VerticalAlignment &align) override
            {
                // do nothing. The View handles this.
            }

            void setPreferredSizeHint(const Size &hint) override
            {
                // nothing to do by default. Most views do not use this.
            }

            void setPreferredSizeMinimum(const Size &limit) override
            {
                // do nothing. The View handles this.
            }

            void setPreferredSizeMaximum(const Size &limit) override
            {
                // do nothing. The View handles this.
            }

            /** Returns the view core associated with this view's parent view.
             *  Returns null if there is no parent view or if the parent does
             * not have a view core associated with it.*/
            std::shared_ptr<ViewCore> getParentViewCore()
            {
                return getViewCoreFromJavaViewRef(_jView.getParent().getRef_());
            }

            Rect adjustAndSetBounds(const Rect &requestedBounds) override
            {
                Rect adjustedBounds = adjustBounds(requestedBounds, RoundType::nearest, RoundType::nearest);

                bdn::java::JObject parent(_jView.getParent());

                if (parent.isNull_()) {
                    // we do not have a parent => we cannot set any position.
                    // Simply do nothing.
                } else {
                    // the parent of all our views is ALWAYS a NativeViewGroup
                    // object.
                    JNativeViewGroup parentView(parent.getRef_());

                    parentView.setChildBounds(getJView(), adjustedBounds.x * _uiScaleFactor,
                                              adjustedBounds.y * _uiScaleFactor, adjustedBounds.width * _uiScaleFactor,
                                              adjustedBounds.height * _uiScaleFactor);
                }

                return adjustedBounds;
            }

            Rect adjustBounds(const Rect &requestedBounds, RoundType positionRoundType,
                              RoundType sizeRoundType) const override
            {
                // align on pixel boundaries
                return Dip::pixelAlign(requestedBounds, _uiScaleFactor, positionRoundType, sizeRoundType);
            }

            double uiLengthToDips(const UILength &uiLength) const override;

            Margin uiMarginToDipMargin(const UIMargin &margin) const override
            {
                return Margin(uiLengthToDips(margin.top), uiLengthToDips(margin.right), uiLengthToDips(margin.bottom),
                              uiLengthToDips(margin.left));
            }

            Size calcPreferredSize(const Size &availableSpace = Size::none()) const override
            {
                int widthSpec;
                int heightSpec;

                if (std::isfinite(availableSpace.width) && canAdjustWidthToAvailableSpace()) {
                    widthSpec =
                        JView::MeasureSpec::makeMeasureSpec(std::lround(stableScaledRoundDown(availableSpace.width,
                                                                                              _uiScaleFactor) *
                                                                        _uiScaleFactor), // round DOWN to the closest
                                                                                         // pixel then scale up and
                                                                                         // round to the nearest integer
                                                            JView::MeasureSpec::atMost);
                } else
                    widthSpec = JView::MeasureSpec::makeMeasureSpec(0, JView::MeasureSpec::unspecified);

                if (std::isfinite(availableSpace.height) && canAdjustHeightToAvailableSpace()) {
                    heightSpec =
                        JView::MeasureSpec::makeMeasureSpec(std::lround(stableScaledRoundDown(availableSpace.height,
                                                                                              _uiScaleFactor) *
                                                                        _uiScaleFactor), // round DOWN to the closest
                                                                                         // pixel then scale up and
                                                                                         // round to the nearest integer
                                                            JView::MeasureSpec::atMost);
                } else
                    heightSpec = JView::MeasureSpec::makeMeasureSpec(0, JView::MeasureSpec::unspecified);

                _jView.measure(widthSpec, heightSpec);

                int width = _jView.getMeasuredWidth();
                int height = _jView.getMeasuredHeight();

                // logInfo("Preferred size of "+std::to_string((int64_t)this)+"
                // "+String(typeid(*this).name())+" :
                // ("+std::to_string(width)+"x"+std::to_string(height)+");
                // available:
                // ("+std::to_string(availableWidth)+"x"+std::to_string(availableHeight)+")
                // ");

                // android uses physical pixels. So we must convert to DIPs.
                Size prefSize(width / _uiScaleFactor, height / _uiScaleFactor);

                std::shared_ptr<const View> view = getOuterViewIfStillAttached();
                if (view != nullptr) {
                    prefSize.applyMinimum(view->preferredSizeMinimum);
                    prefSize.applyMaximum(view->preferredSizeMaximum);
                }

                return prefSize;
            }

            void layout() override
            {
                // do nothing by default. Most views do not have subviews.
            }

            bool canMoveToParentView(std::shared_ptr<View> newParentView) const override { return true; }

            void moveToParentView(std::shared_ptr<View> newParentView) override
            {
                std::shared_ptr<View> outer = getOuterViewIfStillAttached();
                if (outer != nullptr) {
                    std::shared_ptr<View> parent = outer->getParentView();

                    if (newParentView != parent) {
                        // Parent has changed. Remove the view from its current
                        // super view.
                        dispose();
                        _addToParent(newParentView);
                    }
                }
            }

            void dispose() override { _removeFromParent(); }

            /** Called when the user clicks on the view.
             *
             *  The default implementation does nothing.
             * */
            virtual void clicked() {}

            /** Returns the current UI scale factor. This depends on the pixel
             *density of the current display. On high DPI displays the scale
             *factor is >1 to scale up UI elements to use more physical pixels.
             *
             *  Scale factor 1 means a "medium" DPI setting of 160 dpi.
             *
             *  Note that the scale factor can also be <1 if the display has a
             *very low dpi (lower than 160).
             *
             *  Also note that the scale factor can change at runtime if the
             *view switches to another display.
             *
             **/
            double getUIScaleFactor() const { return _uiScaleFactor; }

            /** Changes the UI scale factor of this view and all its child
             * views. See getUIScaleFactor()
             * */
            void setUIScaleFactor(double scaleFactor)
            {
                if (scaleFactor != _uiScaleFactor) {
                    _uiScaleFactor = scaleFactor;

                    std::shared_ptr<View> view = getOuterViewIfStillAttached();
                    std::list<std::shared_ptr<View>> childList = view->getChildViews();

                    for (std::shared_ptr<View> &child : childList) {
                        std::shared_ptr<ViewCore> childCore = std::dynamic_pointer_cast<ViewCore>(child->getViewCore());

                        if (childCore != nullptr)
                            childCore->setUIScaleFactor(scaleFactor);
                    }
                }
            }

          protected:
            /** Returns true if the view can adjust its width to fit into
                a certain size of available space.

                If this returns false then calcPreferredSize will ignore the
                availableWidth parameter.

                The default implementation returns false.
            */
            virtual bool canAdjustWidthToAvailableSpace() const { return false; }

            /** Returns true if the view can adjust its height to fit into
                a certain size of available space.

                If this returns false then calcPreferredSize will ignore the
                availableHeight parameter.

                The default implementation returns false.
            */
            virtual bool canAdjustHeightToAvailableSpace() const { return false; }

            virtual double getFontSizeDips() const
            {
                // most views do not have a font attached on android. So for
                // most views we return the sem size.
                return getSemSizeDips();
            }

            double getEmSizeDips() const
            {
                if (_emDipsIfInitialized == -1)
                    _emDipsIfInitialized = getFontSizeDips();

                return _emDipsIfInitialized;
            }

            double getSemSizeDips() const;

          private:
            void _addToParent(std::shared_ptr<View> parent)
            {
                if (parent != nullptr) {
                    std::shared_ptr<IParentViewCore> parentCore =
                        std::dynamic_pointer_cast<IParentViewCore>(parent->getViewCore());
                    if (parentCore == nullptr)
                        throw ProgrammingError("Internal error: parent of bdn::android::ViewCore "
                                               "either does not have a core, or its core does not "
                                               "support child views.");

                    parentCore->addChildJView(_jView);

                    setUIScaleFactor(parentCore->getUIScaleFactor());
                }
            }

            void _removeFromParent()
            {
                std::shared_ptr<View> view = getOuterViewIfStillAttached();
                std::shared_ptr<View> parent;
                if (view != nullptr)
                    parent = view->getParentView();

                if (parent == nullptr)
                    return; // no parent – nothing to do

                std::shared_ptr<IParentViewCore> parentCore =
                    std::dynamic_pointer_cast<IParentViewCore>(parent->getViewCore());
                if (parentCore != nullptr) {
                    // XXX: Rather unfortunate – removeAllChildViews() is BFS
                    // and so parent core is no longer set when removing
                    // multiple levels of views. Either change
                    // removeAllChildViews()/_deinitCore() to DFS or change the
                    // removal mechanism to use the platform parent.
                    parentCore->removeChildJView(_jView);
                }
            }

            std::weak_ptr<View> _outerViewWeak;

          private:
            mutable JView _jView;
            double _uiScaleFactor;

            Margin _defaultPixelPadding;

            mutable double _emDipsIfInitialized = -1;
            mutable double _semDipsIfInitialized = -1;
        };
    }
}
