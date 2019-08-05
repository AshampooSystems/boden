path: tree/master/framework/ui/modules/lottieview/include/bdn/ui/lottie
source: View.h

# LottieView

Provides a simple way to display vector animations made with Adobe After Effects.

## Declaration

```C++
namespace bdn::ui::lottie {
	class View : public ui::View
}
```

## Example

```C++
#include <bdn/ui/lottie/View.h>
// ...
auto lottieView  = std::make_shared<bdn::ui::lottie::View>();
lottieView->url = "resource://main/images/animation.json";
lottieView->running = true;
lottieView->loop = true;
```

## Properties

* **[Property](../../foundation/property.md)<std::string\> url**
	
	URL pointing to the animation file. (Please refer to [Using Resources](../../../guides/fundamentals/resources.md#using-resources) for more information on how to bundle resources with your app.)

* **[Property](../../foundation/property.md)<bool\> running**

	Whether the animation is running. Set this to `true` to start the animation or to `false` to stop it.

* **[Property](../../foundation/property.md)<bool\> loop**

	Whether the animation should repeat in a loop. Set this to `true` if you want a looping animation.

## Relationships

Inherits from [`View`](../../ui/view.md).

