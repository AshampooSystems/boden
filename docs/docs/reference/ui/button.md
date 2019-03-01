# Button

Simple Button control

## Example

```C++
auto button = std::make_shared<Button>();
button->label = "Click me!";
textField->onClick() += [](auto event) {
  // Do something on submit
};
```

## Header
```C++
#include <bdn/Button.h>
```

## Relationships

Inherits from [`View`](view.md).


## Properties

* **Property<String\> label;**
	
	The Buttons label

## Events

* **ISyncNotifier<const ClickEvent &\> &onClick()**

	A notifier for click events. Subscribe to this notifier if you want to be notified when the user clicks the Button
