#pragma once

namespace bdn
{
    class UIProvider;
}

#include <bdn/Factory.h>
#include <bdn/UIContext.h>
#include <bdn/View.h>
#include <bdn/ViewCore.h>
#include <bdn/ViewCoreTypeNotSupportedError.h>

namespace bdn
{
    class UIProvider : public bdn::Factory<std::shared_ptr<ViewCore>, std::shared_ptr<UIProvider>>,
                       public std::enable_shared_from_this<UIProvider>
    {
      public:
        UIProvider() {}

        using ContextStack = std::vector<std::shared_ptr<UIContext>>;

        virtual String getName() const = 0;

        std::shared_ptr<ViewCore> createViewCore(const String &coreTypeName);

        template <class CoreType>
        static std::shared_ptr<ViewCore> makeCore(const std::shared_ptr<UIProvider> &uiProvider)
        {
            auto viewCore = std::make_shared<CoreType>(uiProvider);
            viewCore->init();
            return viewCore;
        }

        template <class CoreType, class ViewType> void registerCoreType()
        {
            registerConstruction(ViewType::coreTypeName, &makeCore<CoreType>);
        }

      public:
        static void pushContext(std::shared_ptr<bdn::UIContext> &context);
        static void popContext();

        template <class T> static std::shared_ptr<T> getContextStackTop()
        {
            auto stack = contextStack();
            if (stack->empty()) {
                throw std::runtime_error("No UIContext available");
            }
            if (auto top = std::dynamic_pointer_cast<T>(stack->back())) {
                return top;
            }

            throw std::runtime_error("Invalid UIContext");
        }

      private:
        static ContextStack *contextStack();
    };

    std::shared_ptr<UIProvider> defaultUIProvider();
}