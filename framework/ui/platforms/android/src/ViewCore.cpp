#include <bdn/android/ViewCore.h>

#include <cmath>

namespace bdn::android
{
    void ViewCore::init()
    {
        // int bgColor = 0xFF000000 + (std::rand() % 0x00FFFFFF);
        //_jView.setBackgroundColor(bgColor);

        _uiScaleFactor = 1;

        visible.onChange() += [=](auto &property) {
            _jView.setVisibility(property.get() ? wrapper::View::Visibility::visible
                                                : wrapper::View::Visibility::invisible);
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
        _jView.setTag(JavaObject(tag.getRef_()));
    }

    Size ViewCore::sizeForSpace(Size availableSpace) const
    {
        int widthSpec;
        int heightSpec;

        if (std::isfinite(availableSpace.width) && canAdjustWidthToAvailableSpace()) {
            widthSpec = wrapper::View::MeasureSpec::makeMeasureSpec(availableSpace.width * _uiScaleFactor,
                                                                    wrapper::View::MeasureSpec::atMost);
        } else {
            widthSpec = wrapper::View::MeasureSpec::makeMeasureSpec(0, wrapper::View::MeasureSpec::unspecified);
        }

        if (std::isfinite(availableSpace.height) && canAdjustHeightToAvailableSpace()) {
            heightSpec = wrapper::View::MeasureSpec::makeMeasureSpec(availableSpace.height * _uiScaleFactor,
                                                                     wrapper::View::MeasureSpec::atMost);
        } else {
            heightSpec = wrapper::View::MeasureSpec::makeMeasureSpec(0, wrapper::View::MeasureSpec::unspecified);
        }

        _jView.measure(widthSpec, heightSpec);

        int width = _jView.getMeasuredWidth();
        int height = _jView.getMeasuredHeight();

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

    void ViewCore::updateGeometry()
    {
        bdn::JavaObject parent(_jView.getParent().getRef_());
        Rect rGeometry = geometry;

        if (parent.isNull_()) {
            auto thisViewAsNativeGroup = getJViewAS<wrapper::NativeViewGroup>();
            if (thisViewAsNativeGroup.isInstanceOf_(wrapper::NativeViewGroup::javaClass())) {
                thisViewAsNativeGroup.setSize((int)(rGeometry.width * _uiScaleFactor),
                                              (int)(rGeometry.height * _uiScaleFactor));
            }
        } else {
            wrapper::NativeViewGroup parentView(parent.getRef_());
            if (parentView.isInstanceOf_(wrapper::NativeViewGroup::javaClass())) {
                parentView.setChildBounds(getJView(), (int)(rGeometry.x * _uiScaleFactor),
                                          (int)(rGeometry.y * _uiScaleFactor), (int)(rGeometry.width * _uiScaleFactor),
                                          (int)(rGeometry.height * _uiScaleFactor));
            } else {
                _jView.layout((int)(rGeometry.x * _uiScaleFactor), (int)(rGeometry.y * _uiScaleFactor),
                              (int)(rGeometry.width * _uiScaleFactor), (int)(rGeometry.height * _uiScaleFactor));
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
            }
            if (viewTag.isInstanceOf_(bdn::java::wrapper::NativeStrongPointer::getStaticClass_())) {
                bdn::java::wrapper::NativeStrongPointer viewTagPtr(viewTag.getRef_());
                return std::dynamic_pointer_cast<ViewCore>(viewTagPtr.getBdnBasePointer());
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
