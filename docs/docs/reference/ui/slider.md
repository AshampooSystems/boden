path: tree/master/framework/ui/include/bdn/
source: Slider.h

# Slider

A slider

## Declaration

```C++
class Slider : public View
```

## Example

```C++
#include <bdn/Slider.h>
// ...
auto slider = std::make_shared<Slider>();
slider->value = 0.5;
```

## Properties

* **[Property](../foundation/property.md)<double\> value**

	The position of the slider, between 0.0 and 1.0

## Relationships

Inherits from: [View](view.md)
 