#pragma once

#include <bdn/ClickEvent.h>
#include <bdn/Notifier.h>
#include <bdn/UIUtil.h>
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
        Notifier<const ClickEvent &> &onClick();

      protected:
        void bindViewCore() override;

      private:
        Notifier<const ClickEvent &> _onClick;
        WeakCallback<void()>::Receiver _clickCallbackReceiver;

      public:
        class Core
        {
          public:
            Property<String> label;

          public:
            WeakCallback<void()> _clickCallback;
        };
    };
}
