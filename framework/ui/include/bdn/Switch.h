#pragma once

#include <bdn/ClickEvent.h>
#include <bdn/SimpleNotifier.h>
#include <bdn/TriState.h>
#include <bdn/UIUtil.h>
#include <bdn/View.h>

namespace bdn
{
    namespace detail
    {
        VIEW_CORE_REGISTRY_DECLARATION(Switch)
    }

    class Switch : public View
    {
      public:
        Property<String> label;
        Property<bool> on;

      public:
        Switch(std::shared_ptr<ViewCoreFactory> viewCoreFactory = nullptr);

      public:
        TriState state() const;

      public:
        ISyncNotifier<const ClickEvent &> &onClick();

      protected:
        void bindViewCore() override;

      protected:
        std::shared_ptr<SimpleNotifier<const ClickEvent &>> _onClick;

      public:
        class Core
        {
          public:
            Property<String> label;
            Property<bool> on;

          public:
            WeakCallback<void()> _clickCallback;
        };
    };
}
