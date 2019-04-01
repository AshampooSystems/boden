path: tree/master/framework/ui/include/bdn/
source: Window.h

# Window

A Window

> Note: On Mac OSX: A real window

> Note: On IOS: There can be only one!

> Note: On Android: There can be only one!

## Declaration

```C++
class Window : public View
```

## Example

```C++
#include <bdn/Window.h>
// ...
auto window = std::make_shared<Window>();
window->title = "My Awesome App";
```

## Properties

* **Property<Rect\> contentGeometry**

	*(Read-only)* The "safe" area that the content view will occupy. 

* **Property<String\> title**

	The title of the Window. 

	On Android this will be reflected in the Activities Toolbar. 

* **Property<std::shared_ptr<View\>\> content**

	The content view of the window

* **Property<Orientation\> allowedOrientations**

	The orientations the UI will rotate too.

* **Property<Orientation\> currentOrientation**

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
 