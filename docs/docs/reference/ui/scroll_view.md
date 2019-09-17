path: tree/master/framework/ui/include/bdn/ui
source: ScrollView.h

# ScrollView

A container view that can scroll its content view.

## Declaration

```C++
namespace bdn::ui {
	class ScrollView : public View
}
```

## Example

```C++
#include <bdn/ui/ScrollView.h>
#include <bdn/ui/ImageView.h>
// ...
auto image = std::make_shared<ImageView>();
image->url = "https:://www.xyz.com/some_huge_image.png";

auto scrollView = std::make_shared<ScrollView>();
scrollView->contentView = image;
```

## Properties

* **[Property](../foundation/property.md)<std::shared_ptr<[View](view.md)\>\> contentView**

	The content view to be scrolled.

* **[Property](../foundation/property.md)<bool\> verticalScrollingEnabled**

	Whether vertical scrolling is enabled.

* **[Property](../foundation/property.md)<bool\> horizontalScrollingEnabled**

	Whether horizontal scrolling is enabled.

* **[Property](../foundation/property.md)<[Rect](../foundation/rect.md)\> visibleClientRect**

	The currently visible part of the content view.


## Actions

* **void scrollClientRectToVisible(const [Rect](../foundation/rect.md) &area)**

	Scrolls the view so that the area of the content view is visible.

## Relationships

Inherits from: [View](view.md)
 