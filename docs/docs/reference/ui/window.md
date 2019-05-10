path: tree/master/framework/ui/include/bdn/ui
source: Window.h

# Window

Represents a native window. On iOS and Android there can be only a single `Window` instance at a time currently.

## Declaration

```C++
namespace bdn::ui {
    class Window : public View
}
```

## Example

```C++
#include <bdn/ui/Window.h>
// ...
auto window = std::make_shared<Window>();
window->title = "My Awesome App";
```

## Properties

* **const [Property](../foundation/property.md)<Rect\> contentGeometry**

	*(Read-only)* The "safe" area that the content view will occupy. 

* **[Property](../foundation/property.md)<[String](../foundation/string.md)\> title**

	The title of the Window. 

	On Android this will be reflected in the Activities Toolbar. 

* **[Property](../foundation/property.md)<std::shared_ptr<[View](view.md)\>\> contentView**

	The content view of the window.

* **[Property](../foundation/property.md)<Orientation\> allowedOrientations**

	The orientations the UI will rotate too.

* **[Property](../foundation/property.md)<Orientation\> currentOrientation**

	The current orientation of the device.

## Enums

```C++
enum Orientation : int
{
    Portrait = 0x1,
    LandscapeLeft = 0x2,
    LandscapeRight = 0x4,
    PortraitUpsideDown = 0x8,
    LandscapeMask = (LandscapeLeft | LandscapeRight),
    PortraitMask = (Portrait | PortraitUpsideDown),
    All = (LandscapeMask | PortraitMask)
};
```

## Relationships

Inherits from: [View](view.md)
 