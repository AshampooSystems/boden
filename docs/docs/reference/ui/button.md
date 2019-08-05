path: tree/master/framework/ui/include/bdn/ui
source: Button.h

# Button

A simple button with a text label.

## Declaration

```C++
namespace bdn::ui {
	class Button : public View
}
```

## Example

```C++
#include <bdn/ui/Button.h>
// ...
auto button = std::make_shared<Button>();
button->label = "Click me!";
button->onClick() += [](auto event) {
  // Do something on submit
};
```

## Properties

* **[Property](../foundation/property.md)<std::string\> label**
	
	The button's label.

## Events

* **[Notifier](../foundation/notifier.md)<const [ClickEvent](click_event.md) &\> &onClick()**

	A notifier for click events. Subscribe to this notifier if you want to be notified when the user clicks the button.

## Relationships

Inherits from [`View`](view.md).

