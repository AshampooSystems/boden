#ifndef BDN_TextField_H_
#define BDN_TextField_H_

#include <bdn/View.h>
#include <bdn/ITextFieldCore.h>
#include <bdn/SubmitEvent.h>
#include <bdn/SimpleNotifier.h>

namespace bdn
{

    /** A simple single line text field */
    class TextField : public View
    {
      public:
        TextField() { _onSubmit = newObj<SimpleNotifier<const SubmitEvent &>>(); }

        /** Static function that returns the type name for #TextField objects.
         */
        static String getTextFieldCoreTypeName() { return "bdn.TextFieldCore"; }

        String getCoreTypeName() const override { return getTextFieldCoreTypeName(); }

        /** The text field's text */
        BDN_VIEW_PROPERTY(String, text, setText, ITextFieldCore, influencesNothing());

        /** Informs observers of the onSubmit() notifier about a submit event.
         */
        void submit()
        {
            bdn::SubmitEvent event(this);
            onSubmit().notify(event);
        }

        /** A notifier for submit events. Subscribe to this notifier if you want
           to be notified about submit events posted to the text field. Submit
           events are posted when the user presses the Enter key or when
           submit() is called programmatically.
        */
        ISyncNotifier<const SubmitEvent &> &onSubmit() { return *_onSubmit; }

      private:
        P<SimpleNotifier<const SubmitEvent &>> _onSubmit;
    };
}

#endif
