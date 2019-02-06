#pragma once

#include <bdn/TriState.h>
#include <bdn/View.h>
#include <bdn/ClickEvent.h>
#include <bdn/CheckboxCore.h>
#include <bdn/SimpleNotifier.h>

namespace bdn
{

    /** A control providing the user with an on/off choice.

        The Checkbox class implements a checkbox control. If the target platform
        does not provide a native checkbox widget, a platform-specific
        substitution provided by the framework will be displayed instead.

        Note that Android does not support mixed state checkboxes.
     */
    class Checkbox : public View
    {
      public:
        Property<String> label;
        Property<TriState> state;

      public:
        Checkbox();

        /** A notifier for click events. Subscribe to this notifier if you want
           to be notified about click events. Click events are posted when the
           user clicks on the checkbox or checkbox label.
         */
        ISyncNotifier<const ClickEvent &> &onClick();

        /** Returns the name of Checkbox core objects */
        static constexpr char coreTypeName[] = "bdn.CheckboxCore";

        /** Returns the core type name */
        String getCoreTypeName() const override { return coreTypeName; }

      protected:
        std::shared_ptr<SimpleNotifier<const ClickEvent &>> _onClick;
    };
}
