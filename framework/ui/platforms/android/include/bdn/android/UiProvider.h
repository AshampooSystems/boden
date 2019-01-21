#pragma once

namespace bdn
{
    namespace android
    {

        class UiProvider;
    }
}

#include <bdn/IUiProvider.h>
#include <bdn/LayoutCoordinator.h>

#include <bdn/android/ViewCore.h>
#include <bdn/android/JTextView.h>

#include <cmath>

namespace bdn
{
    namespace android
    {

        class UiProvider : public Base, virtual public IUiProvider
        {
          public:
            UiProvider()
            {
                _semDips = -1;
                _layoutCoordinator = std::make_shared<LayoutCoordinator>();
            }

            String getName() const override;

            std::shared_ptr<IViewCore> createViewCore(const String &coreTypeName, std::shared_ptr<View> view) override;

            double getSemSizeDips(ViewCore &viewCore);

            /** Returns the layout coordinator that is used by view cores
             * created by this UI provider.*/
            std::shared_ptr<LayoutCoordinator> getLayoutCoordinator() { return _layoutCoordinator; }

            static std::shared_ptr<UiProvider> get();

          private:
            double _semDips;
            std::shared_ptr<LayoutCoordinator> _layoutCoordinator;
        };
    }
}
