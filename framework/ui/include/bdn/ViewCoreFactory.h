#pragma once

namespace bdn
{
    class ViewCoreFactory;
}

#include <bdn/Factory.h>
#include <bdn/UIContext.h>
#include <bdn/View.h>
#include <bdn/ViewCoreTypeNotSupportedError.h>

namespace bdn
{
    class ViewCoreFactory : public bdn::Factory<std::shared_ptr<View::Core>, std::shared_ptr<ViewCoreFactory>>,
                            public std::enable_shared_from_this<ViewCoreFactory>
    {
      public:
        using ContextStack = std::vector<std::shared_ptr<UIContext>>;

      public:
        std::shared_ptr<View::Core> createViewCore(const std::type_info &viewType);

      public:
        template <class CoreType, class ViewType> void registerCoreType()
        {
            registerConstruction(typeid(ViewType).name(), &makeCore<CoreType>);
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
        template <class CoreType>
        static std::shared_ptr<View::Core> makeCore(const std::shared_ptr<ViewCoreFactory> &ViewCoreFactory)
        {
            auto viewCore = std::make_shared<CoreType>(ViewCoreFactory);
            viewCore->init();
            return viewCore;
        }

      private:
        static ContextStack *contextStack();
    };
}
