
#include <bdn/ios/ContainerViewCore.hh>

namespace bdn
{
    namespace ios
    {

        UIView *ContainerViewCore::_createContainer(std::shared_ptr<ContainerView> outer)
        {
            return [[UIView alloc] initWithFrame:CGRectMake(0, 0, 0, 0)];
        }

        ContainerViewCore::ContainerViewCore(std::shared_ptr<ContainerView> outer)
            : ViewCore(outer, _createContainer(outer))
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

        bool ContainerViewCore::canAdjustToAvailableWidth() const { return true; }

        bool ContainerViewCore::canAdjustToAvailableHeight() const { return true; }
    }
}
