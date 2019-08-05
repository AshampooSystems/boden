#pragma once

#include <bdn/ui/ClickEvent.h>
#include <bdn/ui/TriState.h>
#include <bdn/ui/View.h>
#include <bdn/ui/ViewUtilities.h>

namespace bdn::ui
{
    namespace detail
    {
        VIEW_CORE_REGISTRY_DECLARATION(Checkbox)
    }

    class Checkbox : public View
    {
      public:
        Property<std::string> label;
        Property<TriState> state;

      public:
        Checkbox(std::shared_ptr<ViewCoreFactory> viewCoreFactory = nullptr);

      public:
        Notifier<const ClickEvent &> &onClick();

      protected:
        void bindViewCore() override;

      protected:
        Notifier<const ClickEvent &> _onClick;

      private:
        WeakCallback<void()>::Receiver _clickCallbackReceiver;

      public:
        class Core
        {
            friend class Checkbox;

          public:
            using ClickCallback = std::function<void()>;

          public:
            Property<TriState> state;
            Property<std::string> label;

          protected:
            WeakCallback<void()> _clickCallback;
        };
    };
}
