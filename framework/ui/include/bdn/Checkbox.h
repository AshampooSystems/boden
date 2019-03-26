#pragma once

#include <bdn/CheckboxCore.h>
#include <bdn/ClickEvent.h>
#include <bdn/SimpleNotifier.h>
#include <bdn/TriState.h>
#include <bdn/View.h>

namespace bdn
{
    namespace detail
    {
        VIEW_CORE_REGISTRY_DECLARATION(Checkbox)
    }

    class Checkbox : public View
    {
      public:
        Property<String> label;
        Property<TriState> state;

      public:
        Checkbox(std::shared_ptr<ViewCoreFactory> viewCoreFactory = nullptr);

        ISyncNotifier<const ClickEvent &> &onClick();

        static constexpr char coreTypeName[] = "bdn.CheckboxCore";

        String viewCoreTypeName() const override { return coreTypeName; }

      protected:
        std::shared_ptr<SimpleNotifier<const ClickEvent &>> _onClick;

        void bindViewCore() override;

      private:
        WeakCallback<void()>::Receiver _clickCallbackReceiver;
    };
}
