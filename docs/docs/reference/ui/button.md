# Button

Simple Button control

## Declaration

```C++
class Button : public View
```

## Example

```C++
#include <bdn/Button.h>
// ...
auto button = std::make_shared<Button>();
button->label = "Click me!";
button->onClick() += [](auto event) {
  // Do something on submit
};
```

## Properties

* **Property<String\> label;**
	
	The Buttons label

## Events

* **ISyncNotifier<const ClickEvent &\> &onClick()**

	A notifier for click events. Subscribe to this notifier if you want to be notified when the user clicks the Button

## Relationships

Inherits from [`View`](view.md).

