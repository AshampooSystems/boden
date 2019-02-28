#include <bdn/android/UIProvider.h>
#include <bdn/android/ViewCore.h>

#include <cmath>

namespace bdn
{
    namespace android
    {
        void ViewCore::init()
        {
            // int bgColor = 0xFF000000 + (std::rand() % 0x00FFFFFF);
            //_jView.setBackgroundColor(bgColor);

            _uiScaleFactor = 1; // will be updated in _addToParent

            visible.onChange() += [=](auto va) {
                _jView.setVisibility(va->get() ? JView::Visibility::visible : JView::Visibility::invisible);
            };

            geometry.onChange() += [=](auto va) {
                bdn::java::JObject parent(_jView.getParent());
                Rect rGeometry = va->get();

                if (parent.isNull_()) {
                    auto thisViewAsNativeGroup = getJViewAS<JNativeViewGroup>();
                    if (thisViewAsNativeGroup.isInstanceOf_(JNativeViewGroup::javaClass())) {
                        thisViewAsNativeGroup.setSize(rGeometry.width * _uiScaleFactor,
                                                      rGeometry.height * _uiScaleFactor);
                    }
                } else {

                    JNativeViewGroup parentView(parent.getRef_());
                    if (parentView.isInstanceOf_(JNativeViewGroup::javaClass())) {
                        parentView.setChildBounds(getJView(), rGeometry.x * _uiScaleFactor,
                                                  rGeometry.y * _uiScaleFactor, rGeometry.width * _uiScaleFactor,
                                                  rGeometry.height * _uiScaleFactor);
                    } else {
                        _jView.layout(rGeometry.x * _uiScaleFactor, rGeometry.y * _uiScaleFactor,
                                      rGeometry.width * _uiScaleFactor, rGeometry.height * _uiScaleFactor);
                    }
                }
            };

            JNativeViewCoreLayoutChangeListener layoutChangeListener;
            getJView().addOnLayoutChangeListener(layoutChangeListener);
        }

        ViewCore::~ViewCore()
        {
            // remove the the reference to ourselves from the java-side
            // view object. Note that we hold a strong reference to the
            // java-side object, So we know that the reference to the
            // java-side object is still valid.
            _jView.setTag(bdn::java::JObject(bdn::java::Reference()));
        }

        void ViewCore::initTag()
        {
            auto tag = bdn::java::NativeWeakPointer(std::dynamic_pointer_cast<ViewCore>(shared_from_this()));
            _jView.setTag(tag);
        }

        std::shared_ptr<View> ViewCore::getOuterViewIfStillAttached() const { return _outerViewWeak.lock(); }

        std::shared_ptr<ViewCore> ViewCore::getParentViewCore()
        {
            return getViewCoreFromJavaViewRef(_jView.getParent().getRef_());
        }

        Size ViewCore::sizeForSpace(Size availableSpace) const
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

            return prefSize;
        }

        bool ViewCore::canMoveToParentView(std::shared_ptr<View> newParentView) const { return true; }

        void ViewCore::moveToParentView(std::shared_ptr<View> newParentView)
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

        void ViewCore::dispose() { _removeFromParent(); }

        void ViewCore::layoutChange(int left, int top, int right, int bottom, int oldLeft, int oldTop, int oldRight,
                                    int oldBottom)
        {
            Rect newRect = {(double)left, (double)top, (double)(right - left), (double)(bottom - top)};
            Rect oldRect = {(double)oldLeft, (double)oldTop, (double)(oldRight - oldLeft),
                            (double)(oldBottom - oldTop)};

            Rect prevGeom = geometry.get();

            if (oldRect == prevGeom && newRect != prevGeom) {
                geometry = newRect;
            }
        }

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
        double ViewCore::getUIScaleFactor() const { return _uiScaleFactor; }

        /** Changes the UI scale factor of this view and all its child
         * views. See getUIScaleFactor()
         * */
        void ViewCore::setUIScaleFactor(double scaleFactor)
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

        double ViewCore::getFontSizeDips() const
        {
            // most views do not have a font attached on android. So for
            // most views we return the sem size.
            return getSemSizeDips();
        }

        double ViewCore::getEmSizeDips() const
        {
            if (_emDipsIfInitialized == -1)
                _emDipsIfInitialized = getFontSizeDips();

            return _emDipsIfInitialized;
        }

        double ViewCore::getSemSizeDips() const
        {
            if (_semDipsIfInitialized == -1)
                _semDipsIfInitialized = UIProvider::get()->getSemSizeDips(*const_cast<ViewCore *>(this));

            return _semDipsIfInitialized;
        }

        void ViewCore::_addToParent(std::shared_ptr<View> parent)
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

        void ViewCore::_removeFromParent()
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

        std::shared_ptr<ViewCore> getViewCoreFromJavaViewRef(const java::Reference &javaViewRef)
        {
            if (!javaViewRef.isNull()) {
                JView view(javaViewRef);
                bdn::java::JObject viewTag(view.getTag());
                if (viewTag.isInstanceOf_(bdn::java::NativeWeakPointer::getStaticClass_())) {
                    bdn::java::NativeWeakPointer viewTagPtr(viewTag.getRef_());
                    return std::static_pointer_cast<ViewCore>(viewTagPtr.getPointer().lock());
                } else if (viewTag.isInstanceOf_(bdn::java::JNativeStrongPointer::getStaticClass_())) {
                    bdn::java::JNativeStrongPointer viewTagPtr(viewTag.getRef_());
                    return std::static_pointer_cast<ViewCore>(viewTagPtr.getPointer_());
                }
            }

            return nullptr;
        }

        void ViewCore::scheduleLayout() {}
    }
}
