# TextView

A display only text view 

## Declaration

```C++
class TextView : public View
```

## Example

```C++
#include <bdn/TextView.h>
// ...
auto label = std::make_shared<TextView>();
label->text = "Hello World";
```

## Properties

* **Property<String\> text**

	The views text

* **Property<bool\> wrap**

	If true, the text will wrap if it does not fit in its width.

## Relationships

Inherits from: [View](view.md)
 