path: tree/master/framework/ui/include/bdn/
source: ImageView.h

# ImageView

Displays an image on the user interface.

## Declaration

```C++
class ImageView : public View
```

## Example

```C++
#include <bdn/ImageView.h>
// ...
auto imageView = std::make_shared<ImageView>();
imageView->url = 'https://via.placeholder.com/350x150';
```

## Properties

* **Property<[String](../foundation/string.md)\> url**
	
	The URL of the image to be displayed.

## Relationships

Inherits from [`View`](view.md).

