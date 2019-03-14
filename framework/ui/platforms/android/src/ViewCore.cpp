#include <bdn/android/UIProvider.h>
#include <bdn/android/ViewCore.h>

#include <cmath>

namespace bdn::android
{
    void ViewCore::init()
    {
        // int bgColor = 0xFF000000 + (std::rand() % 0x00FFFFFF);
        //_jView.setBackgroundColor(bgColor);

        _uiScaleFactor = 1;

        visible.onChange() += [=](auto va) {
            _jView.setVisibility(va->get() ? wrapper::View::Visibility::visible : wrapper::View::Visibility::invisible);
        };

        geometry.onChange() += [=](auto) { updateGeometry(); };

        wrapper::NativeViewCoreLayoutChangeListener layoutChangeListener;
        getJView().addOnLayoutChangeListener(layoutChangeListener);

        initTag();
    }

    ViewCore::~ViewCore()
    {
        // remove the the reference to ourselves from the java-side
        // view object. Note that we hold a strong reference to the
        // java-side object, So we know that the reference to the
        // java-side object is still valid.
        _jView.setTag(bdn::JavaObject(bdn::java::Reference()));
    }

    void ViewCore::initTag()
    {
        auto tag = bdn::java::wrapper::NativeWeakPointer(shared_from_this());
        _jView.setTag(tag);
    }

    Size ViewCore::sizeForSpace(Size availableSpace) const
    {
        int widthSpec;
        int heightSpec;

        if (std::isfinite(availableSpace.width) && canAdjustWidthToAvailableSpace()) {
            widthSpec = wrapper::View::MeasureSpec::makeMeasureSpec(
                std::lround(stableScaledRoundDown(availableSpace.width, _uiScaleFactor) * _uiScaleFactor),

                wrapper::View::MeasureSpec::atMost);
        } else
            widthSpec = wrapper::View::MeasureSpec::makeMeasureSpec(0, wrapper::View::MeasureSpec::unspecified);

        if (std::isfinite(availableSpace.height) && canAdjustHeightToAvailableSpace()) {
            heightSpec = wrapper::View::MeasureSpec::makeMeasureSpec(
                std::lround(stableScaledRoundDown(availableSpace.height, _uiScaleFactor) * _uiScaleFactor),
                wrapper::View::MeasureSpec::atMost);
        } else
            heightSpec = wrapper::View::MeasureSpec::makeMeasureSpec(0, wrapper::View::MeasureSpec::unspecified);

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

    void ViewCore::layoutChange(int left, int top, int right, int bottom, int oldLeft, int oldTop, int oldRight,
                                int oldBottom)
    {
        Rect newRect = {(double)left, (double)top, (double)(right - left), (double)(bottom - top)};
        Rect oldRect = {(double)oldLeft, (double)oldTop, (double)(oldRight - oldLeft), (double)(oldBottom - oldTop)};

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
        bool changed = scaleFactor != _uiScaleFactor;
        _uiScaleFactor = scaleFactor;
        if (changed) {
            updateGeometry();
            updateChildren();
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

    void ViewCore::updateGeometry()
    {
        bdn::JavaObject parent(_jView.getParent());
        Rect rGeometry = geometry;

        if (parent.isNull_()) {
            auto thisViewAsNativeGroup = getJViewAS<wrapper::NativeViewGroup>();
            if (thisViewAsNativeGroup.isInstanceOf_(wrapper::NativeViewGroup::javaClass())) {
                thisViewAsNativeGroup.setSize(rGeometry.width * _uiScaleFactor, rGeometry.height * _uiScaleFactor);
            }
        } else {

            wrapper::NativeViewGroup parentView(parent.getRef_());
            if (parentView.isInstanceOf_(wrapper::NativeViewGroup::javaClass())) {
                parentView.setChildBounds(getJView(), rGeometry.x * _uiScaleFactor, rGeometry.y * _uiScaleFactor,
                                          rGeometry.width * _uiScaleFactor, rGeometry.height * _uiScaleFactor);
            } else {
                _jView.layout(rGeometry.x * _uiScaleFactor, rGeometry.y * _uiScaleFactor,
                              rGeometry.width * _uiScaleFactor, rGeometry.height * _uiScaleFactor);
            }
        }
    }

    std::shared_ptr<ViewCore> viewCoreFromJavaViewRef(const java::Reference &javaViewRef)
    {
        if (!javaViewRef.isNull()) {
            wrapper::View view(javaViewRef);
            bdn::JavaObject viewTag(view.getTag());
            if (viewTag.isInstanceOf_(bdn::java::wrapper::NativeWeakPointer::getStaticClass_())) {
                bdn::java::wrapper::NativeWeakPointer viewTagPtr(viewTag.getRef_());
                return std::dynamic_pointer_cast<ViewCore>(viewTagPtr.getPointer().lock());
            } else if (viewTag.isInstanceOf_(bdn::java::wrapper::NativeStrongPointer::getStaticClass_())) {
                bdn::java::wrapper::NativeStrongPointer viewTagPtr(viewTag.getRef_());
                return std::dynamic_pointer_cast<ViewCore>(viewTagPtr.getPointer_());
            }
        }

        return nullptr;
    }

    void ViewCore::scheduleLayout()
    {
        getJView().requestLayout();
        markDirty();
    }

    void ViewCore::updateChildren()
    {
        double scaleFactor = getUIScaleFactor();

        visitInternalChildren([scaleFactor](auto child) {
            if (auto androidCore = std::dynamic_pointer_cast<android::ViewCore>(child)) {
                androidCore->setUIScaleFactor(scaleFactor);
            }
        });
    }
}
