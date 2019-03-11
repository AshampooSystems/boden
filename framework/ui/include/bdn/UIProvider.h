#pragma once

namespace bdn
{
    class UIProvider;
}

#include <bdn/Factory.h>
#include <bdn/View.h>
#include <bdn/ViewCore.h>
#include <bdn/ViewCoreTypeNotSupportedError.h>

namespace bdn
{
    class UIProvider : public bdn::Factory<std::shared_ptr<ViewCore>>
    {
      public:
        /** Returns the name of the UI provider. This is intended for logging
         * and diagnostics.*/
        virtual String getName() const = 0;

        std::shared_ptr<ViewCore> createViewCore(const String &coreTypeName)
        {
            auto core = create(coreTypeName);

            if (!core) {
                throw ViewCoreTypeNotSupportedError(coreTypeName);
            }

            return *core;
        }

      protected:
        template <class CoreType> static std::shared_ptr<ViewCore> makeCore()
        {
            auto viewCore = std::make_shared<CoreType>();
            viewCore->init();
            return viewCore;
        }

        template <class CoreType, class ViewType> void registerCoreType()
        {
            registerConstruction(ViewType::coreTypeName, &makeCore<CoreType>);
        }
    };

    std::shared_ptr<UIProvider> defaultUIProvider();
}
