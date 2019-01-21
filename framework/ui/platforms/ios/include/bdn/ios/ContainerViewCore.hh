#pragma once

#include <bdn/ContainerView.h>

#import <bdn/ios/ViewCore.hh>

namespace bdn
{
    namespace ios
    {

        class ContainerViewCore : public ViewCore
        {
          private:
            static UIView *_createContainer(std::shared_ptr<ContainerView> outer);

          public:
            ContainerViewCore(std::shared_ptr<ContainerView> outer);

            Size calcPreferredSize(const Size &availableSpace) const override;

            void layout() override;

          protected:
            bool canAdjustToAvailableWidth() const override;

            bool canAdjustToAvailableHeight() const override;
        };
    }
}
