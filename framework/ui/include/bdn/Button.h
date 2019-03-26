#pragma once

#include <bdn/ButtonCore.h>
#include <bdn/ClickEvent.h>
#include <bdn/SimpleNotifier.h>
#include <bdn/View.h>

namespace bdn
{
    namespace detail
    {
        VIEW_CORE_REGISTRY_DECLARATION(Button)
    }

    class Button : public View
    {
      public:
        Property<String> label;

      public:
        Button(std::shared_ptr<ViewCoreFactory> viewCoreFactory = nullptr);

      public:
        ISyncNotifier<const ClickEvent &> &onClick();

      public:
        String viewCoreTypeName() const override { return coreTypeName; }

      protected:
        void bindViewCore() override;

      public:
        static constexpr char coreTypeName[] = "bdn.ButtonCore";

      private:
        std::shared_ptr<SimpleNotifier<const ClickEvent &>> _onClick;
        WeakCallback<void()>::Receiver _clickCallbackReceiver;
    };
}
