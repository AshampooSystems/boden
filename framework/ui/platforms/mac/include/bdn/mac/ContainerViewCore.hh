#pragma once

#include <bdn/ContainerView.h>
#import <bdn/mac/ChildViewCore.hh>
#include <bdn/mac/IParentViewCore.h>

namespace bdn
{
    namespace mac
    {

        class ContainerViewCore : public ChildViewCore, virtual public IParentViewCore
        {
          private:
            static NSView *_createContainer(std::shared_ptr<ContainerView> outer);

          public:
            ContainerViewCore(std::shared_ptr<ContainerView> outer) : ChildViewCore(outer, _createContainer(outer)) {}

            Size calcPreferredSize(const Size &availableSpace) const override
            {
                // call the outer container's preferred size calculation

                std::shared_ptr<ContainerView> outerView =
                    std::dynamic_pointer_cast<ContainerView>(getOuterViewIfStillAttached());
                if (outerView != nullptr)
                    return outerView->calcContainerPreferredSize(availableSpace);
                else
                    return Size(0, 0);
            }

            void layout() override
            {
                // call the outer container's layout function

                std::shared_ptr<ContainerView> outerView =
                    std::dynamic_pointer_cast<ContainerView>(getOuterViewIfStillAttached());
                if (outerView != nullptr) {
                    std::shared_ptr<ViewLayout> layout = outerView->calcContainerLayout(outerView->size);
                    layout->applyTo(outerView);
                }
            }
        };
    }
}
