path: tree/master/framework/ui/include/bdn/
source: ScrollView.h

# ScrollView

A Container that can scroll its content view

## Declaration

```C++
class ScrollView : public View
```

## Example

```C++
#include <bdn/ScrollView.h>
#include <bdn/ImageView.h>
// ...
auto image = std::make_shared<ImageView>();
image->url = "https:://www.xyz.com/some_huge_image.png";

auto scrollView = std::make_shared<ScrollView>();
scrollView->content = image;
```

## Properties

* **Property<std::shared_ptr<View\>\> content**

	The view to be scrolled

* **Property<bool\> verticalScrollingEnabled**

	If true, vertical scrolling is enabled

* **Property<bool\> horizontalScrollingEnabled**

	If true, horizontal scrolling is enabled

* **Property<Rect\> visibleClientRect**

	The currently visible part of the content view 


## Actions

* **void scrollClientRectToVisible(const Rect &area)**

	Scroll the view so that the *area* of the content view is visible

## Relationships

Inherits from: [View](view.md)
 