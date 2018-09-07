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
                return uiLength.value * UiProvider::get().getSemSizeDips(
                                            *const_cast<ViewCore *>(this));

            default:
                throw InvalidArgumentError("Invalid UiLength unit passed to "
                                           "ViewCore::uiLengthToDips: " +
                                           std::to_string((int)uiLength.unit));
            }
        }

        double ViewCore::getSemSizeDips() const
        {
            if (_semDipsIfInitialized == -1)
                _semDipsIfInitialized = UiProvider::get().getSemSizeDips(
                    *const_cast<ViewCore *>(this));

            return _semDipsIfInitialized;
        }

        void ViewCore::needLayout(View::InvalidateReason reason)
        {
            P<View> pOuterView = getOuterViewIfStillAttached();
            if (pOuterView != nullptr) {
                P<UiProvider> pProvider =
                    tryCast<UiProvider>(pOuterView->getUiProvider());
                if (pProvider != nullptr)
                    pProvider->getLayoutCoordinator()->viewNeedsLayout(
                        pOuterView);
            }
        }

        void ViewCore::childSizingInfoInvalidated(View *pChild)
        {
            P<View> pOuterView = getOuterViewIfStillAttached();
            if (pOuterView != nullptr) {
                pOuterView->invalidateSizingInfo(
                    View::InvalidateReason::childSizingInfoInvalidated);
                pOuterView->needLayout(
                    View::InvalidateReason::childSizingInfoInvalidated);
            }
        }
    }
}
