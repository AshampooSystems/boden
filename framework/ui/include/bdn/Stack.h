#pragma once

#include <bdn/FixedView.h>
#include <bdn/View.h>

#include <deque>

namespace bdn
{
    namespace detail
    {
        VIEW_CORE_REGISTRY_DECLARATION(Stack)
    }

    class Stack : public View
    {
      public:
        static constexpr char coreTypeName[] = "bdn.Stack";

      public:
        Stack(std::shared_ptr<ViewCoreFactory> viewCoreFactory = nullptr);
        ~Stack() override;

        void pushView(std::shared_ptr<View> view, String title);
        void popView();

      public:
        std::list<std::shared_ptr<View>> childViews() override;
        String viewCoreTypeName() const override { return coreTypeName; }

      protected:
        void bindViewCore() override;
    };
}
