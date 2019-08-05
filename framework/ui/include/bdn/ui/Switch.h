#pragma once

#include <bdn/Notifier.h>
#include <bdn/ui/ClickEvent.h>
#include <bdn/ui/TriState.h>
#include <bdn/ui/View.h>
#include <bdn/ui/ViewUtilities.h>

namespace bdn::ui
{
    namespace detail
    {
        VIEW_CORE_REGISTRY_DECLARATION(Switch)
    }

    class Switch : public View
    {
      public:
        Property<std::string> label;
        Property<bool> on;

      public:
        Switch(std::shared_ptr<ViewCoreFactory> viewCoreFactory = nullptr);

      public:
        TriState state() const;

      public:
        Notifier<const ClickEvent &> &onClick();

      protected:
        void bindViewCore() override;

      protected:
        Notifier<const ClickEvent &> _onClick;

      public:
        class Core
        {
          public:
            Property<std::string> label;
            Property<bool> on;

          public:
            WeakCallback<void()> _clickCallback;
        };
    };
}
