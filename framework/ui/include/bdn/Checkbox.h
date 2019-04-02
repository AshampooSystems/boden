#pragma once

#include <bdn/ClickEvent.h>
#include <bdn/TriState.h>
#include <bdn/UIUtil.h>
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
            Property<String> label;

          protected:
            WeakCallback<void()> _clickCallback;
        };
    };
}
