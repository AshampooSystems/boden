path: tree/master/framework/ui/include/bdn/ui
source: Label.h

# Label

A read-only text label.

## Declaration

```C++
namespace bdn::ui {
	class Label : public View
}
```

## Example

```C++
#include <bdn/ui/Label.h>
// ...
auto label = std::make_shared<Label>();
label->text = "Hello World";
```

## Properties

* **[Property](../foundation/property.md)<[Text](text.md)\> text**

	The label's text.

* **[Property](../foundation/property.md)<bool\> wrap**

	If true, the text will wrap if it exceeds the available width.

	Defaults to `false`

* **[Property](../foundation/property.md)<[TextOverflow](text_overflow.md)\> textOverflow**

	Specifies the way text is cut if it does not fit inside the view.
	Defaults to `TextOverflow::EllipsisTail`.

## Relationships

Inherits from: [View](view.md)
 