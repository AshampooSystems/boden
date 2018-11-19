#include <bdn/init.h>
#include <bdn/android/ViewCore.h>

#include <bdn/android/UiProvider.h>

#include <cmath>

namespace bdn
{
    namespace android
    {

        double ViewCore::uiLengthToDips(const UiLength &uiLength) const
        {
            switch (uiLength.unit) {
            case UiLength::Unit::none:
                return 0;

            case UiLength::Unit::dip:
                return uiLength.value;

            case UiLength::Unit::em:
                return uiLength.value * getEmSizeDips();

            case UiLength::Unit::sem:
                return uiLength.value * UiProvider::get().getSemSizeDips(*const_cast<ViewCore *>(this));

            default:
                throw InvalidArgumentError("Invalid UiLength unit passed to "
                                           "ViewCore::uiLengthToDips: " +
                                           std::to_string((int)uiLength.unit));
            }
        }

        double ViewCore::getSemSizeDips() const
        {
            if (_semDipsIfInitialized == -1)
                _semDipsIfInitialized = UiProvider::get().getSemSizeDips(*const_cast<ViewCore *>(this));

            return _semDipsIfInitialized;
        }

        void ViewCore::needLayout(View::InvalidateReason reason)
        {
            P<View> outerView = getOuterViewIfStillAttached();
            if (outerView != nullptr) {
                P<UiProvider> provider = tryCast<UiProvider>(outerView->getUiProvider());
                if (provider != nullptr)
                    provider->getLayoutCoordinator()->viewNeedsLayout(outerView);
            }
        }

        void ViewCore::childSizingInfoInvalidated(View *child)
        {
            P<View> outerView = getOuterViewIfStillAttached();
            if (outerView != nullptr) {
                outerView->invalidateSizingInfo(View::InvalidateReason::childSizingInfoInvalidated);
                outerView->needLayout(View::InvalidateReason::childSizingInfoInvalidated);
            }
        }
    }
}
