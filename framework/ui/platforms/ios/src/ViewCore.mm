
#import <bdn/ios/ViewCore.hh>

#include <bdn/ProgrammingError.h>

namespace bdn::ios
{
    ViewCore::ViewCore(const std::shared_ptr<bdn::UIProvider> &uiProvider, id<UIViewWithFrameNotification> uiView)
        : bdn::ViewCore(uiProvider)
    {
        _view = (UIView<UIViewWithFrameNotification> *)uiView;

        //_view.backgroundColor = [UIColor colorWithHue:drand48() saturation:1.0 brightness:1.0 alpha:1.0];
    }

    void ViewCore::init()
    {
        [_view setViewCore:std::dynamic_pointer_cast<ViewCore>(shared_from_this())];

        geometry.onChange() += [=](auto va) { this->onGeometryChanged(va->get()); };

        visible.onChange() += [&view = this->_view](auto va) { view.hidden = !va->get(); };
    }

    ViewCore::~ViewCore() { [_view setViewCore:std::weak_ptr<ViewCore>()]; }

    void ViewCore::frameChanged() { geometry = iosRectToRect(_view.frame); }

    void ViewCore::onGeometryChanged(Rect newGeometry) { _view.frame = rectToIosRect(newGeometry); }

    UIView *ViewCore::uiView() const { return _view; }

    Size ViewCore::sizeForSpace(Size availableSpace) const
    {
        CGSize constraintSize = UILayoutFittingCompressedSize;

        // systemLayoutSizeFittingSize will clip the return value to the
        // constraint size. So we only pass the available space if the
        // view can actually adjust itself to the available space.
        if (std::isfinite(availableSpace.width) && canAdjustToAvailableWidth()) {
            constraintSize.width = availableSpace.width;
            if (constraintSize.width < 0) {
                constraintSize.width = 0;
            }
        }
        if (std::isfinite(availableSpace.height) && canAdjustToAvailableHeight()) {
            constraintSize.height = availableSpace.height;
            if (constraintSize.height < 0) {
                constraintSize.height = 0;
            }
        }

        CGSize iosSize = [_view systemLayoutSizeFittingSize:constraintSize];

        Size size = iosSizeToSize(iosSize);

        if (size.width < 0) {
            size.width = 0;
        }
        if (size.height < 0) {
            size.height = 0;
        }

        return size;
    }

    bool ViewCore::canMoveToParentView(std::shared_ptr<View> newParentView) const { return true; }

    void ViewCore::addChildViewCore(const std::shared_ptr<ViewCore> &core) { [_view addSubview:core->uiView()]; }

    void ViewCore::removeFromUISuperview() { [_view removeFromSuperview]; }

    bool ViewCore::canAdjustToAvailableWidth() const { return false; }

    bool ViewCore::canAdjustToAvailableHeight() const { return false; }

    double ViewCore::getFontSize() const
    {
        // most views do not have a font size attached to them on ios.
        // UILabel and UIButton are pretty much the only ones.
        // Those should override this function.
        // In the default implementation we simply return the system
        // font size.
        return getSemSizeDips();
    }

    double ViewCore::getEmSizeDips() const
    {
        if (_emDipsIfInitialized == -1) {
            _emDipsIfInitialized = getFontSize();
        }

        return _emDipsIfInitialized;
    }

    double ViewCore::getSemSizeDips() const
    {
        if (_semDipsIfInitialized == -1) {
            _semDipsIfInitialized = UIProvider::get()->getSemSizeDips();
        }

        return _semDipsIfInitialized;
    }

    void ViewCore::scheduleLayout() { [_view setNeedsLayout]; }
}
