
#include <bdn/android/ContainerViewCore.h>

namespace bdn
{
    namespace android
    {
        ContainerViewCore::ContainerViewCore(std::shared_ptr<ContainerView> outer)
            : ViewCore(outer, ViewCore::createAndroidViewClass<JNativeViewGroup>(outer))
        {}

        Size ContainerViewCore::calcPreferredSize(const Size &availableSpace) const
        {
            // call the outer container's preferred size calculation

            std::shared_ptr<ContainerView> outerView =
                std::dynamic_pointer_cast<ContainerView>(getOuterViewIfStillAttached());
            if (outerView != nullptr)
                return outerView->calcContainerPreferredSize(availableSpace);
            else
                return Size(0, 0);
        }

        void ContainerViewCore::layout()
        {
            // call the outer container's layout function

            std::shared_ptr<ContainerView> outerView =
                std::dynamic_pointer_cast<ContainerView>(getOuterViewIfStillAttached());
            if (outerView != nullptr) {
                std::shared_ptr<ViewLayout> layout = outerView->calcContainerLayout(outerView->size);
                layout->applyTo(outerView);
            }
        }

        Rect ContainerViewCore::adjustAndSetBounds(const Rect &requestedBounds)
        {
            Rect adjustedBounds = ViewCore::adjustAndSetBounds(requestedBounds);

            JNativeViewGroup thisGroup(getJView().getRef_());

            double scaleFactor = getUIScaleFactor();

            thisGroup.setSize(std::lround(adjustedBounds.width * scaleFactor),
                              std::lround(adjustedBounds.height * scaleFactor));

            return adjustedBounds;
        }

        double ContainerViewCore::getUIScaleFactor() const { return ViewCore::getUIScaleFactor(); }

        void ContainerViewCore::addChildJView(JView childJView)
        {
            JNativeViewGroup parentGroup(getJView().getRef_());

            parentGroup.addView(childJView);
        }

        void ContainerViewCore::removeChildJView(JView childJView)
        {
            JNativeViewGroup parentGroup(getJView().getRef_());
            parentGroup.removeView(childJView);
        }
    }
}
