
#import <bdn/ios/ViewCore.hh>

#include <bdn/ProgrammingError.h>

namespace bdn
{
    namespace ios
    {

        ViewCore::ViewCore(std::shared_ptr<View> outerView, id<UIViewWithFrameNotification> uiView)
        {
            [uiView setViewCore:this];
            _outerViewWeak = outerView;
            _view = (UIView *)uiView;

            _addToParent(outerView->getParentView());

            geometry.onChange() += [=](auto va) { this->onGeometryChanged(va->get()); };

            visible.onChange() += [&view = this->_view](auto va) { view.hidden = !va->get(); };
        }

        ViewCore::~ViewCore() { dispose(); }

        void ViewCore::frameChanged() { geometry = iosRectToRect(_view.frame); }

        void ViewCore::onGeometryChanged(Rect newGeometry) { _view.frame = rectToIosRect(newGeometry); }

        std::shared_ptr<View> ViewCore::getOuterViewIfStillAttached() const { return _outerViewWeak.lock(); }

        UIView *ViewCore::getUIView() const { return _view; }

        Size ViewCore::sizeForSpace(Size availableSpace) const
        {
            CGSize constraintSize = UILayoutFittingCompressedSize;

            // systemLayoutSizeFittingSize will clip the return value to the
            // constraint size. So we only pass the available space if the
            // view can actually adjust itself to the available space.
            if (std::isfinite(availableSpace.width) && canAdjustToAvailableWidth()) {
                constraintSize.width = availableSpace.width;
                if (constraintSize.width < 0)
                    constraintSize.width = 0;
            }
            if (std::isfinite(availableSpace.height) && canAdjustToAvailableHeight()) {
                constraintSize.height = availableSpace.height;
                if (constraintSize.height < 0)
                    constraintSize.height = 0;
            }

            CGSize iosSize = [_view systemLayoutSizeFittingSize:constraintSize];

            Size size = iosSizeToSize(iosSize);

            if (size.width < 0)
                size.width = 0;
            if (size.height < 0)
                size.height = 0;

            return size;
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

        void ViewCore::dispose()
        {
            removeFromUISuperview();
            _view = nil;
        }

        void ViewCore::addChildViewCore(ViewCore *viewCore) { [_view addSubview:viewCore->getUIView()]; }

        void ViewCore::removeFromUISuperview() { [_view removeFromSuperview]; }

        bool ViewCore::canAdjustToAvailableWidth() const { return false; }

        bool ViewCore::canAdjustToAvailableHeight() const { return false; }

        void ViewCore::_addToParent(std::shared_ptr<View> parentView)
        {
            if (parentView == nullptr) {
                // top level window. Nothing to do.
                return;
            }

            std::shared_ptr<bdn::ViewCore> parentCore = parentView->getViewCore();
            if (parentCore == nullptr) {
                // this should not happen. The parent MUST have a core -
                // otherwise we cannot initialize ourselves.
                throw ProgrammingError("bdn::ios::ViewCore constructed for a view whose "
                                       "parent does not have a core.");
            }

            std::dynamic_pointer_cast<ViewCore>(parentCore)->addChildViewCore(this);
        }

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
            if (_emDipsIfInitialized == -1)
                _emDipsIfInitialized = getFontSize();

            return _emDipsIfInitialized;
        }

        double ViewCore::getSemSizeDips() const
        {
            if (_semDipsIfInitialized == -1)
                _semDipsIfInitialized = UIProvider::get()->getSemSizeDips();

            return _semDipsIfInitialized;
        }

        void ViewCore::scheduleLayout() { [_view setNeedsLayout]; }
    }
}
