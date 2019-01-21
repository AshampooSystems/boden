#pragma once

#include <bdn/constants.h>
#include <bdn/View.h>
#include <bdn/ClickEvent.h>
#include <bdn/ISwitchCore.h>
#include <bdn/SimpleNotifier.h>

namespace bdn
{

    /** A simple on/off toggle adapting its appearance to the target platform

        The Toggle class implements a control providing the user with a binary
       on or off choice. On mobile platforms, it appears as a Switch whereas on
       desktop platforms it appears as a Checkbox. If the control's label is
       set, it appears to the left of the switch control.

        Use toggles for presenting users with a binary on/off switch which
       automatically conforms to the user interface best practices on all
       platforms.
     */
    class Toggle : public View
    {
      public:
        Property<String> label;
        Property<bool> on;

        Property<TriState> state;

      public:
        Toggle();

        /** A notifier for click events. Subscribe to this notifier if you want
           to be notified about click events. Click events are posted when the
           user clicks on the toggle. If the toggle is displayed as a switch,
           the label is not clickable. If the toggle is displayed as a checkbox,
            the label is clickable.
         */
        ISyncNotifier<const ClickEvent &> &onClick();

        static String getToggleCoreTypeName()
        {
            // UIProvider decides whether this will be a CheckboxCore<Toggle>
            // or a SwitchCore<Toggle>
            return "bdn.ToggleCore";
        }

        String getCoreTypeName() const override { return getToggleCoreTypeName(); }

      protected:
        std::shared_ptr<SimpleNotifier<const ClickEvent &>> _onClick;
    };
}
