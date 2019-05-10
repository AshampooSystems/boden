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

* **[Property](../foundation/property.md)<[String](../foundation/string.md)\> text**

	The label's text.

* **[Property](../foundation/property.md)<bool\> wrap**

	If true, the text will wrap if it exceeds the available width.

## Relationships

Inherits from: [View](view.md)
 