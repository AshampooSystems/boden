path: tree/master/framework/ui/include/bdn/ui
source: Checkbox.h

# Checkbox

A control providing the user with an on/off choice.

In contrast to [`Switch`](switch.md), `Checkbox` is commonly used in scenarios where the user's on or off choice does not take effect immediately and an additional confirmation is required for the setting to take effect, e.g. by pushing an OK button.

If the target platform does not provide a native checkbox widget, a platform-specific substitution provided by the framework will be displayed instead.

## Declaration

```C++
namespace bdn::ui {
	class Checkbox : public View
}
```

## Example

```C++
#include <bdn/ui/Checkbox.h>
// ...
auto checkbox = std::make_shared<Checkbox>();
checkbox->label = "Click me!";
checkbox->state = TriState::on;
checkbox->onClick() += [](auto event) {
  // Do something on submit
};
```

## Properties

* **[Property](../foundation/property.md)<std::string\> label**
	
	The checkbox's label.

* **[Property](../foundation/property.md)<[TriState](tri_state.md)\> state**
	
	A [TriState](tri_state.md) representing the checkbox's state. Note that Android does not support mixed state checkboxes. Checkboxes in mixed state will be displayed as off on Android.

* **[Property](../foundation/property.md)<bool\> checked**
	
	A boolean representing the checkbox's state. It mirrors and updates the `state` property when changed.

## Events

* **[Notifier](../foundation/notifier.md)<const [ClickEvent](click_event.md) &\> &onClick()**

	A notifier for click events. Subscribe to this notifier if you want to be notified when the user clicks the checkbox.

## Relationships

Inherits from [`View`](view.md).

