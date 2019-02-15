#pragma once

namespace bdn
{
    namespace android
    {

        class UIProvider;
    }
}

#include <bdn/LayoutCoordinator.h>
#include <bdn/UIProvider.h>

#include <bdn/android/JTextView.h>
#include <bdn/android/ViewCore.h>

#include <cmath>

namespace bdn
{
    namespace android
    {

        class UIProvider : public Base, virtual public bdn::UIProvider
        {
          public:
            UIProvider();

            String getName() const override;

            double getSemSizeDips(ViewCore &viewCore);

            /** Returns the layout coordinator that is used by view cores
             * created by this UI provider.*/
            std::shared_ptr<LayoutCoordinator> getLayoutCoordinator() { return _layoutCoordinator; }

            static std::shared_ptr<UIProvider> get();

          private:
            template <class CoreType, class ViewType>
            static std::shared_ptr<IViewCore> makeAndroidCore(std::shared_ptr<View> view)
            {
                auto realView = std::dynamic_pointer_cast<ViewType>(view);
                auto p = std::make_shared<CoreType>(realView);

                p->getJView().setTag(bdn::java::NativeWeakPointer(p));
                return p;
            }

            template <class CoreType, class ViewType> void registerAndroidCoreType()
            {
                registerConstruction(ViewType::coreTypeName, &makeAndroidCore<CoreType, ViewType>);
            }

          private:
            double _semDips;
            std::shared_ptr<LayoutCoordinator> _layoutCoordinator;
        };
    }
}
