#pragma once

#include <bdn/AppControllerBase.h>
#include <bdn/IUiProvider.h>

namespace bdn
{

    class UiAppControllerBase : public AppControllerBase
    {
      public:
        using AppControllerBase::AppControllerBase;

        virtual std::shared_ptr<IUiProvider> getUiProvider() { return getDefaultUiProvider(); }

        static std::shared_ptr<UiAppControllerBase> get()
        {
            return std::dynamic_pointer_cast<UiAppControllerBase>(_globalAppController());
        }
    };
}