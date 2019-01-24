#pragma once

#include <bdn/UiProvider.h>
#include <bdn/LayoutCoordinator.h>

namespace bdn
{
    namespace mac
    {

        class UiProvider : public Base, virtual public bdn::UiProvider
        {
          public:
            UiProvider();

            String getName() const override;

            static std::shared_ptr<UiProvider> get();

            double getSemSizeDips() const { return _semDips; }

            /** Returns the layout coordinator that is used by view cores
             * created by this UI provider.*/
            std::shared_ptr<LayoutCoordinator> getLayoutCoordinator() { return _layoutCoordinator; }

          private:
            double _semDips;

            std::shared_ptr<LayoutCoordinator> _layoutCoordinator;
        };
    }
}
