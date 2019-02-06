
#include <bdn/android/ViewCore.h>

#include <bdn/android/UIProvider.h>

#include <cmath>

namespace bdn
{
    namespace android
    {

        double ViewCore::uiLengthToDips(const UILength &uiLength) const
        {
            switch (uiLength.unit) {
            case UILength::Unit::none:
                return 0;

            case UILength::Unit::dip:
                return uiLength.value;

            case UILength::Unit::em:
                return uiLength.value * getEmSizeDips();

            case UILength::Unit::sem:
                return uiLength.value * UIProvider::get()->getSemSizeDips(*const_cast<ViewCore *>(this));

            default:
                throw InvalidArgumentError("Invalid UILength unit passed to "
                                           "ViewCore::uiLengthToDips: " +
                                           std::to_string((int)uiLength.unit));
            }
        }

        double ViewCore::getSemSizeDips() const
        {
            if (_semDipsIfInitialized == -1)
                _semDipsIfInitialized = UIProvider::get()->getSemSizeDips(*const_cast<ViewCore *>(this));

            return _semDipsIfInitialized;
        }

        void ViewCore::needLayout(View::InvalidateReason reason)
        {
            std::shared_ptr<View> outerView = getOuterViewIfStillAttached();
            if (outerView != nullptr) {
                std::shared_ptr<UIProvider> provider =
                    std::dynamic_pointer_cast<UIProvider>(outerView->getUIProvider());
                if (provider != nullptr)
                    provider->getLayoutCoordinator()->viewNeedsLayout(outerView);
            }
        }

        void ViewCore::childSizingInfoInvalidated(std::shared_ptr<View> child)
        {
            std::shared_ptr<View> outerView = getOuterViewIfStillAttached();
            if (outerView != nullptr) {
                outerView->invalidateSizingInfo(View::InvalidateReason::childSizingInfoInvalidated);
                outerView->needLayout(View::InvalidateReason::childSizingInfoInvalidated);
            }
        }
    }
}
