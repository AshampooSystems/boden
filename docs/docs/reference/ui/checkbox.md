path: tree/master/framework/ui/include/bdn/
source: Checkbox.h

# Checkbox

A control providing the user with an on/off choice.

If the target platform does not provide a native checkbox widget, a platform-specific substitution provided by the framework will be displayed instead.

## Declaration

```C++
class Checkbox : public View
```

## Example

```C++
#include <bdn/Checkbox.h>
// ...
auto checkbox = std::make_shared<Checkbox>();
checkbox->label = "Click me!";
checkbox->state = TriState::on;
checkbox->onClick() += [](auto event) {
  // Do something on submit
};
```

## Properties

* **Property<String\> label**
	
	The checkbox's label.

* **Property<TriState\> state**
	
	A [TriState](tri_state.md) representing the checkbox's state. Note that Android does not support mixed state checkboxes. Checkboxes in mixed state will be displayed as off on Android.

## Events

* **ISyncNotifier<const ClickEvent &\> &onClick()**

	A notifier for click events. Subscribe to this notifier if you want to be notified when the user clicks the Button.

## Relationships

Inherits from [`View`](view.md).

