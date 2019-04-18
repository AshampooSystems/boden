path: tree/master/framework/ui/include/bdn/
source: Switch.h

# Switch

A control providing the user with an on/off choice.

In contrast to [`Checkbox`](checkbox.md), `Switch` is commonly used in scenarios where the user's on or off choice takes effect immediately and does not require further confirmation.

## Declaration

```C++
class Switch : public View
```

## Example

```C++
#include <bdn/Switch.h>
// ...
auto switchControl = std::make_shared<Switch>();
switchControl->label = "A switch can also have a label!";
switchControl->on = true;
switchControl->onClick() += [](auto event) {
  // Do something when the switch is turned off or on
};
```

## Properties

* **[Property](../foundation/property.md)<[String](../foundation/string.md)\> label**
	
	The switch's label.

* **[Property](../foundation/property.md)<bool\> on**
	
	Whether switch is on (`true`) or off (`false`).

## Events

* **[Notifier](../foundation/notifier.md)<const [ClickEvent](click_event.md) &\> &onClick()**

	A notifier for click events. Subscribe to this notifier if you want to be notified when the user clicks the switch.

## Relationships

Inherits from [`View`](view.md).

