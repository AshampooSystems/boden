#pragma once

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
        Property<String> label;
        Property<bool> on;

      public:
        Switch()
        {
            _onClick = std::make_shared<SimpleNotifier<const ClickEvent &>>();
            label.onChange() += View::CorePropertyUpdater<String, IToggleCoreBase>{
                this, &IToggleCoreBase::setLabel, [](auto &inf) { inf.influencesPreferredSize(); }};

            on.onChange() += View::CorePropertyUpdater<bool, ISwitchCore>{this, &ISwitchCore::setOn};
        }

        /** The switch's state, see TriState */
        TriState state() const { return on ? TriState::on : TriState::off; }

        /** A notifier for click events. Subscribe to this notifier if you want
         to be notified about click events. Click events are posted when the
         user clicks on the switch. No click event is posted when the user
         clicks on the switch's label.
         */
        ISyncNotifier<const ClickEvent &> &onClick() { return *_onClick; }

        static constexpr char coreTypeName[] = "bdn.SwitchCore";

        String getCoreTypeName() const override { return coreTypeName; }

      protected:
        std::shared_ptr<SimpleNotifier<const ClickEvent &>> _onClick;
    };
}
