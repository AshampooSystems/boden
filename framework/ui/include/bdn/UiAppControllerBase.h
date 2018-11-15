#pragma once

#include <bdn/AppControllerBase.h>
#include <bdn/IUiProvider.h>

namespace bdn
{

    class UiAppControllerBase : public AppControllerBase
    {
      public:
        using AppControllerBase::AppControllerBase;

        virtual P<IUiProvider> getUiProvider() { return getDefaultUiProvider(); }

        static P<UiAppControllerBase> get() { return dynamic_cast<UiAppControllerBase *>(_getGlobalRef().getPtr()); }
    };
}
