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
            static UIView *_createContainer(ContainerView *pOuter)
            {
                return [[UIView alloc] initWithFrame:CGRectMake(0, 0, 0, 0)];
            }

          public:
            ContainerViewCore(ContainerView *pOuter) : ViewCore(pOuter, _createContainer(pOuter)) {}

            Size calcPreferredSize(const Size &availableSpace) const override
            {
                // call the outer container's preferred size calculation

                P<ContainerView> pOuterView = cast<ContainerView>(getOuterViewIfStillAttached());
                if (pOuterView != nullptr)
                    return pOuterView->calcContainerPreferredSize(availableSpace);
                else
                    return Size(0, 0);
            }

            void layout() override
            {
                // call the outer container's layout function

                P<ContainerView> pOuterView = cast<ContainerView>(getOuterViewIfStillAttached());
                if (pOuterView != nullptr) {
                    P<ViewLayout> pLayout = pOuterView->calcContainerLayout(pOuterView->size());
                    pLayout->applyTo(pOuterView);
                }
            }

          protected:
            bool canAdjustToAvailableWidth() const override { return true; }

            bool canAdjustToAvailableHeight() const override { return true; }
        };
    }
}

#endif
