
#include <bdn/android/ContainerViewCore.h>

namespace bdn
{
    namespace android
    {

        std::shared_ptr<JNativeViewGroup>
        ContainerViewCore::_createJNativeViewGroup(std::shared_ptr<ContainerView> outer)
        {
            // we need to know the context to create the view.
            // If we have a parent then we can get that from the parent's
            // core.
            std::shared_ptr<View> parent = outer->getParentView();
            if (parent == nullptr)
                throw ProgrammingError("ContainerViewCore instance requested for a "
                                       "ContainerView that does not have a parent.");

            std::shared_ptr<ViewCore> parentCore = std::dynamic_pointer_cast<ViewCore>(parent->getViewCore());
            if (parentCore == nullptr)
                throw ProgrammingError("ContainerViewCore instance requested for a "
                                       "ContainerView with core-less parent.");

            JContext context = parentCore->getJView().getContext();

            return std::make_shared<JNativeViewGroup>(context);
        }

        ContainerViewCore::ContainerViewCore(std::shared_ptr<ContainerView> outer)
            : ViewCore(outer, _createJNativeViewGroup(outer))
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

            double scaleFactor = getUiScaleFactor();

            thisGroup.setSize(std::lround(adjustedBounds.width * scaleFactor),
                              std::lround(adjustedBounds.height * scaleFactor));

            return adjustedBounds;
        }

        double ContainerViewCore::getUiScaleFactor() const { return ViewCore::getUiScaleFactor(); }

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
