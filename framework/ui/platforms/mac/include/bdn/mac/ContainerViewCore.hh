#ifndef BDN_MAC_ContainerViewCore_HH_
#define BDN_MAC_ContainerViewCore_HH_

#include <bdn/ContainerView.h>
#import <bdn/mac/ChildViewCore.hh>
#include <bdn/mac/IParentViewCore.h>

namespace bdn
{
    namespace mac
    {

        class ContainerViewCore : public ChildViewCore, BDN_IMPLEMENTS IParentViewCore
        {
          private:
            static NSView *_createContainer(ContainerView *outer);

          public:
            ContainerViewCore(ContainerView *outer) : ChildViewCore(outer, _createContainer(outer)) {}

            Size calcPreferredSize(const Size &availableSpace) const override
            {
                // call the outer container's preferred size calculation

                P<ContainerView> outerView = cast<ContainerView>(getOuterViewIfStillAttached());
                if (outerView != nullptr)
                    return outerView->calcContainerPreferredSize(availableSpace);
                else
                    return Size(0, 0);
            }

            void layout() override
            {
                // call the outer container's layout function

                P<ContainerView> outerView = cast<ContainerView>(getOuterViewIfStillAttached());
                if (outerView != nullptr) {
                    P<ViewLayout> layout = outerView->calcContainerLayout(outerView->size());
                    layout->applyTo(outerView);
                }
            }
        };
    }
}

#endif
