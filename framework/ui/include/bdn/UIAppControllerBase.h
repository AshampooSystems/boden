#pragma once

#include <bdn/AppControllerBase.h>
#include <bdn/UIProvider.h>

namespace bdn
{

    class UIAppControllerBase : public AppControllerBase
    {
      public:
        using AppControllerBase::AppControllerBase;

        virtual std::shared_ptr<UIProvider> getUIProvider() { return getDefaultUIProvider(); }

        static std::shared_ptr<UIAppControllerBase> get()
        {
            return std::dynamic_pointer_cast<UIAppControllerBase>(_globalAppController());
        }
    };
}
