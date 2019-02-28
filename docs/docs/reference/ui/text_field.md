# TextField

A simple single-line editable text field.

## Example

```
std::shared_ptr<TextField> textField = std::make_shared<TextField>();
textField->text = "Hello world!";
textField->onSubmit() += [](const SubmitEvent &event) {
  // Do something on submit
};
```

## Properties

* **Property<String\> text**

	The text displayed in the text field.

## Actions

* **void submit()**

	Informs observers of the onSubmit() notifier about a submit event.

## Events

* **ISyncNotifier<const SubmitEvent &\> &onSubmit()**

	A notifier for submit events. Subscribe to this notifier if you want to be notified about submit events posted to the text field. Submit events are posted when the user presses the Enter key or when submit() is called programmatically.

## Relationships

Inherits from: [View](view.md)

