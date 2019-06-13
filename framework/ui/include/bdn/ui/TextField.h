#pragma once

#include <bdn/Notifier.h>
#include <bdn/ui/SubmitEvent.h>
#include <bdn/ui/View.h>
#include <bdn/ui/ViewUtilities.h>

namespace bdn::ui
{
    namespace detail
    {
        VIEW_CORE_REGISTRY_DECLARATION(TextField)
    }

    class TextField : public View
    {
      public:
        Property<String> text;

      public:
        TextField(std::shared_ptr<ViewCoreFactory> viewCoreFactory = nullptr);

      public:
        void submit();
        Notifier<const SubmitEvent &> &onSubmit();

      protected:
        void bindViewCore() override;

      private:
        Notifier<const SubmitEvent &> _onSubmit;
        WeakCallback<void()>::Receiver _submitCallbackReceiver;

      public:
        class Core
        {
          public:
            Property<String> text;

          public:
            WeakCallback<void()> submitCallback;
        };
    };
}
