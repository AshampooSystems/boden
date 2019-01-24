#pragma once

#include <bdn/UiProvider.h>
#include <bdn/LayoutCoordinator.h>

namespace bdn
{
    namespace ios
    {

        class UiProvider : public Base, virtual public bdn::UiProvider
        {
          public:
            UiProvider();
            UiProvider(UiProvider const &) = delete;
            void operator=(UiProvider const &) = delete;

            String getName() const override;

            static std::shared_ptr<UiProvider> get();

            /** Returns the size of 1 sem in DIPs.*/
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
