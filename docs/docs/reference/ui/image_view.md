path: tree/master/framework/ui/include/bdn/ui
source: ImageView.h

# ImageView

Displays an image on the user interface.

## Declaration

```C++
namespace bdn::ui {
	class ImageView : public View
}
```

## Example

```C++
#include <bdn/ui/ImageView.h>
// ...
auto imageView = std::make_shared<ImageView>();
imageView->url = 'https://via.placeholder.com/350x150';
```

## Properties

* **Property<[String](../foundation/string.md)\> url**
	
	The URL of the image to be displayed. ( see [Using resources](../../guides/fundamentals/resources.md#using-resources) )

## Relationships

Inherits from [`View`](view.md).

