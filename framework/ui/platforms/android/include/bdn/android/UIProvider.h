#pragma once

namespace bdn::android
{
    class UIProvider;
}

#include <bdn/UIProvider.h>

#include <bdn/android/ContextWrapper.h>
#include <bdn/android/ViewCore.h>
#include <bdn/android/wrapper/Context.h>

#include <vector>

namespace bdn::android
{
    class UIProvider : public Base, virtual public bdn::UIProvider
    {
        using WrapperStack = std::vector<ContextWrapper>;

      public:
        UIProvider();

        String getName() const override;

        double getSemSizeDips(ViewCore &core);

        static std::shared_ptr<UIProvider> get();

        static void pushAndroidContext(const ContextWrapper &context);

        static void popAndroidContext();

      private:
        template <class CoreType> static std::shared_ptr<ViewCore> makeAndroidCore()
        {
            auto stack = androidContextStack();
            if (stack->empty()) {
                throw std::runtime_error("Cannot create Core if no Android Context is registered");
            }

            auto core = std::make_shared<CoreType>(stack->back());
            core->init();
            return core;
        }

        template <class CoreType, class ViewType> void registerAndroidCoreType()
        {
            registerConstruction(ViewType::coreTypeName, &makeAndroidCore<CoreType>);
        }

      private:
        static WrapperStack *androidContextStack();

      private:
        double _semDips;
    };
}
