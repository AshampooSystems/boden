#pragma once

#include <bdn/ClickEvent.h>
#include <bdn/SwitchCore.h>
#include <bdn/TriState.h>
#include <bdn/View.h>

#include <bdn/SimpleNotifier.h>

namespace bdn
{

    /** A control that offers an on or off choice

        The Switch class implements a binary state switch offering the user an
        on or off choice.
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
            label.onChange() += View::CorePropertyUpdater<String, SwitchCore>{
                this, &SwitchCore::setLabel, [](auto &inf) { inf.influencesPreferredSize(); }};

            on.onChange() += View::CorePropertyUpdater<bool, SwitchCore>{this, &SwitchCore::setOn};
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
