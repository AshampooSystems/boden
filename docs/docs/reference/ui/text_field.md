path: tree/master/framework/ui/include/bdn/ui
source: TextField.h

# TextField

A simple single-line editable text field.

## Declaration

```C++
namespace bdn::ui {
	class TextField : public View
}
```

## Example

```C++
#include <bdn/ui/TextField.h>
//...
auto textField = std::make_shared<TextField>();
textField->text = "Hello world!";
textField->onSubmit() += [](auto event) {
  // Do something on submit
};
```


## Properties

* **[Property](../foundation/property.md)<[String](../foundation/string.md)\> text**

	The text displayed in the text field.

## Actions

* **void submit()**

	Informs observers of the `onSubmit()` notifier about a submit event.

## Events

* **[Notifier](../foundation/notifier.md)<const [SubmitEvent](submit_event.md) &\> &onSubmit()**

	Notifies subscribers when the <kbd>Enter</kbd> key is pressed by the user.

	Subscribe to this notifier if you want to be notified about submit events posted to the text field. Submit events are posted when the user presses the <kbd>Enter</kbd> key or when `submit()` is called programmatically.

## Relationships

Inherits from: [View](view.md)

