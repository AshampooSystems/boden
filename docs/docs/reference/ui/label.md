path: tree/master/framework/ui/include/bdn/
source: Label.h

# Label

Displays read-only text 

## Declaration

```C++
class Label : public View
```

## Example

```C++
#include <bdn/Label.h>
// ...
auto label = std::make_shared<Label>();
label->text = "Hello World";
```

## Properties

* **Property<String\> text**

	The views text

* **Property<bool\> wrap**

	If true, the text will wrap if it does not fit in its width.

## Relationships

Inherits from: [View](view.md)
 