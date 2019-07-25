#pragma once

#include <optional>

#include <bdn/Font.h>
#include <bdn/Notifier.h>
#include <bdn/ui/SubmitEvent.h>
#include <bdn/ui/View.h>
#include <bdn/ui/ViewUtilities.h>
#include <bdn/ui/textInputTypes.h>

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
        Property<Font> font;
        Property<AutocorrectionType> autocorrectionType = AutocorrectionType::Default;
        Property<ReturnKeyType> returnKeyType = ReturnKeyType::Default;

      public:
        TextField(std::shared_ptr<ViewCoreFactory> viewCoreFactory = nullptr);

      public:
        void submit();
        Notifier<const SubmitEvent &> &onSubmit();

        void focus();

      protected:
        void bindViewCore() override;
        void updateFromStylesheet() override;

      private:
        Notifier<const SubmitEvent &> _onSubmit;
        WeakCallback<void()>::Receiver _submitCallbackReceiver;

      public:
        class Core
        {
          public:
            Property<String> text;
            Property<Font> font;
            Property<AutocorrectionType> autocorrectionType;
            Property<ReturnKeyType> returnKeyType;

          public:
            virtual void focus() = 0;

          public:
            WeakCallback<void()> submitCallback;
        };
    };
}
