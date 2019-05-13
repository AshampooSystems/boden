
#import <bdn/ios/ViewCore.hh>

namespace bdn::ui::ios
{
    ViewCore::ViewCore(const std::shared_ptr<ViewCoreFactory> &viewCoreFactory, id<UIViewWithFrameNotification> uiView)
        : View::Core(viewCoreFactory)
    {
        _view = (UIView<UIViewWithFrameNotification> *)uiView;

        if (View::debugViewEnabled()) {
            _view.layer.borderWidth = 1;
            _view.layer.borderColor = [UIColor blackColor].CGColor;
            _view.backgroundColor = [UIColor colorWithHue:drand48() saturation:1.0 brightness:1.0 alpha:1.0];
        }
    }

    void ViewCore::init()
    {
        [_view setViewCore:shared_from_this<ViewCore>()];

        geometry.onChange() += [=](auto &property) { this->onGeometryChanged(property.get()); };

        visible.onChange() += [&view = this->_view](auto &property) { view.hidden = !property.get(); };
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

    void ViewCore::scheduleLayout() { [_view setNeedsLayout]; }
}
