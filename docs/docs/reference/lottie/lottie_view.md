path: tree/master/framework/lottieview/include/bdn
source: LottieView.h

# LottieView

LottieView is a simple way to display vector animations. 

## Declaration

```C++
class LottieView : public View
```

## Example

```C++
#include <bdn/LottieView.h>
// ...
auto lottieView  = std::make_shared<LottieView>();
lottieView->url = "resource://main/images/animation.json";
lottieView->running = true;
lottieView->loop = true;
```

## Properties

* **[Property](../foundation/property.md)<[String](../foundation/string.md)\> url**
	
	URL to the animation file. ( see [Using resources](../../guides/fundamentals/resources.md#using-resources) )

* **[Property](../foundation/property.md)<bool\> running**

	Start and stop the animation.

* **[Property](../foundation/property.md)<bool\> loop**

	If true the animation will loop.

## Relationships

Inherits from [`View`](../ui/view.md).

