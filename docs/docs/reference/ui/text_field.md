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

* **[Property](../foundation/property.md)<std::string\> text**

	The text displayed in the text field.

* **[Property](../foundation/property.md)<[Text](text.md)\> placeholder**

	The placeholder displayed while the TextField is empty.  

* **[Property](../foundation/property.md)<[AutocorrectionType](autocorrection_type.md)\> autocorrectionType**

	The autocorrection type used by the text field.

	If not set, the platform's default behavior will be used. If set to `AutocorrectionType::Yes`, the text field will display typing suggestions and apply platform-specific autocorrection and autocomplete.

	Note that on Android, the implementation of the software keyboard may override the behavior indicated by the `autocorrectionType` property.

	This property is supported on Android and iOS only.

	!!! warning
		On Android, this function uses the `EditText` widget's input type flag `TYPE_TEXT_FLAG_NO_SUGGESTIONS` to deactivate autocorrection, autocomplete, and typing suggestions. Based on the software keyboard implementation used by the operating system, this flag may or may not have an effect. Setting the `autocorrectionType` property to `AutocorrectionType::No` is hence not a guarantee that autocorrection will be deactivated on Android.

* **[Property](../foundation/property.md)<[ReturnKeyType](return_key_type.md)\> returnKeyType**

	The type of return key to use when displaying a software keyboard.

	Defaults to `ReturnKeyType::Default` which corresponds to input action `IME_ACTION_UNSPECIFIED` on Android and to `UIReturnKeyDefault` on iOS. The default indicates that the operating system should decide what return key type to display. Typically, iOS will show "return" and Android will show "return" or "done" in the return key button. See [`ReturnKeyType`](return_key_type.md) for details.

	This property is supported on Android and iOS only.

## Stylesheet

* **{ "font" : [Font](../foundation/font.md) }**

	Controls the background color of the view.

## Actions

* **void submit()**

	Informs observers of the `onSubmit()` notifier about a submit event.

* **void focus()**

	Sets focus to the text field.

## Events

* **[Notifier](../foundation/notifier.md)<const [SubmitEvent](submit_event.md) &\> &onSubmit()**

	Notifies subscribers when the <kbd>Enter</kbd> key is pressed by the user.

	Subscribe to this notifier if you want to be notified about submit events posted to the text field. Submit events are posted when the user presses the <kbd>Enter</kbd> key or when `submit()` is called programmatically.

## Relationships

Inherits from: [View](view.md)

