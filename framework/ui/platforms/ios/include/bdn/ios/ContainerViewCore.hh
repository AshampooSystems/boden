#ifndef BDN_IOS_ContainerViewCore_HH_
#define BDN_IOS_ContainerViewCore_HH_

#include <bdn/ContainerView.h>

#import <bdn/ios/ViewCore.hh>

namespace bdn
{
    namespace ios
    {

        class ContainerViewCore : public ViewCore
        {
          private:
            static UIView *_createContainer(ContainerView *outer)
            {
                return [[UIView alloc] initWithFrame:CGRectMake(0, 0, 0, 0)];
            }

          public:
            ContainerViewCore(ContainerView *outer) : ViewCore(outer, _createContainer(outer)) {}

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

          protected:
            bool canAdjustToAvailableWidth() const override { return true; }

            bool canAdjustToAvailableHeight() const override { return true; }
        };
    }
}

#endif
