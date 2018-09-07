#ifndef BDN_Switch_H_
#define BDN_Switch_H_

#include <bdn/constants.h>
#include <bdn/ISwitchCore.h>
#include <bdn/View.h>
#include <bdn/ClickEvent.h>

#include <bdn/SimpleNotifier.h>

namespace bdn
{

    /** A control that offers an on or off choice

        The Switch class implements a binary state switch offering the user an
       on or off choice.

        If the target platform does not provide a native switch control, a
       platform-specific substitution provided by the framework will be
       displayed instead.

        Use switches to provide users with a control for switching a single
       option immediately. Do not use switches in cases where the user is
       presented with multiple options and the result of switching requires
       additional confirmation. In these cases, consider using the Checkbox
       class.

        Switches are commonly used on mobile platforms. See the Toggle class for
       a control that adapts its appearance as a switch or checkbox with regard
       to the target platofmr.
     */
    class Switch : public View
    {
      public:
        Switch() { _pOnClick = newObj<SimpleNotifier<const ClickEvent &>>(); }

        /** The switch's label */
        BDN_VIEW_PROPERTY(String, label, setLabel, ISwitchCore,
                          influencesPreferredSize());

        /** Whether the switch is on (true) or off (false) */
        BDN_VIEW_PROPERTY(bool, on, setOn, ISwitchCore, influencesNothing());

        /** The switch's state, see TriState */
        TriState state() const { return on() ? TriState::on : TriState::off; }

        // XXX stateChanged??

        /** A notifier for click events. Subscribe to this notifier if you want
         to be notified about click events. Click events are posted when the
         user clicks on the switch. No click event is posted when the user
         clicks on the switch's label.
         */
        ISyncNotifier<const ClickEvent &> &onClick() { return *_pOnClick; }

        static String getSwitchCoreTypeName()
        {
            // UIProvider decides whether this will be a Switch<SwitchCore>
            // or a Toggle<SwitchCore>.
            return "bdn.SwitchCore";
        }

        String getCoreTypeName() const override
        {
            return getSwitchCoreTypeName();
        }

      protected:
        P<SimpleNotifier<const ClickEvent &>> _pOnClick;
    };
}

#endif // BDN_Switch_H_
