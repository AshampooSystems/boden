#pragma once

#include <bdn/IUiProvider.h>
#include <bdn/LayoutCoordinator.h>

namespace bdn
{
    namespace mac
    {

        class UiProvider : public Base, virtual public IUiProvider
        {
          public:
            UiProvider();

            String getName() const override;

            std::shared_ptr<IViewCore> createViewCore(const String &coreTypeName, std::shared_ptr<View> view) override;

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
