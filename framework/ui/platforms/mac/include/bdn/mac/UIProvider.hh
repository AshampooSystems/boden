#pragma once

#include <bdn/LayoutCoordinator.h>
#include <bdn/UIProvider.h>

namespace bdn
{
    namespace mac
    {

        class UIProvider : public Base, virtual public bdn::UIProvider
        {
          public:
            UIProvider();

            String getName() const override;

            static std::shared_ptr<UIProvider> get();

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
