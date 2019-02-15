#pragma once

#include <bdn/ITextFieldCore.h>
#include <bdn/SimpleNotifier.h>
#include <bdn/SubmitEvent.h>
#include <bdn/View.h>

namespace bdn
{

    /** A simple single line text field */
    class TextField : public View
    {
      public:
        /** The text field's text */
        Property<String> text;

      public:
        TextField();

        /** Static function that returns the type name for #TextField objects.
         */
        static constexpr char coreTypeName[] = "bdn.TextFieldCore";

        String getCoreTypeName() const override { return coreTypeName; }

        /** Informs observers of the onSubmit() notifier about a submit event.
         */
        void submit();

        /** A notifier for submit events. Subscribe to this notifier if you want
           to be notified about submit events posted to the text field. Submit
           events are posted when the user presses the Enter key or when
           submit() is called programmatically.
        */
        ISyncNotifier<const SubmitEvent &> &onSubmit();

      private:
        std::shared_ptr<SimpleNotifier<const SubmitEvent &>> _onSubmit;
    };
}
