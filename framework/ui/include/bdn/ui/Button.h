#pragma once

#include <bdn/Notifier.h>
#include <bdn/ui/ClickEvent.h>
#include <bdn/ui/Text.h>
#include <bdn/ui/View.h>
#include <bdn/ui/ViewUtilities.h>

namespace bdn::ui
{
    namespace detail
    {
        VIEW_CORE_REGISTRY_DECLARATION(Button)
    }

    class Button : public View
    {
      public:
        Property<Text> label;
        Property<std::string> imageURL;

      public:
        Button(std::shared_ptr<ViewCoreFactory> viewCoreFactory = nullptr);

      public:
        Notifier<const ClickEvent &> &onClick();

      protected:
        void bindViewCore() override;
        void updateFromStylesheet() override;

      private:
        Notifier<const ClickEvent &> _onClick;
        WeakCallback<void()>::Receiver _clickCallbackReceiver;

      public:
        class Core
        {
          public:
            Property<Text> label;
            Property<std::string> imageURL;

          public:
            WeakCallback<void()> _clickCallback;
        };
    };
}
