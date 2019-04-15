path: tree/master/framework/ui/include/bdn/
source: NavigationView.h

# NavigationView

Provides the user with a way to navigate between a number of different views where only one view is visible at a time.

## Declaration

```C++
class NavigationView : public View
```

## Example

```C++
#include <bdn/Button.h>
#include <bdn/NavigationView.h>
// ...
auto navigationView = std::make_shared<NavigationView>();
auto firstButton = std::make_shared<Button>();

firstButton->label = "Click me!";
firstButton->onClick() += [=](auto) {
	auto secondButton = std::make_shared<Button>();
	secondButton->label = "Go back";
	secondButton->onClick() += [=](auto) { navigationView->popView(); }
	navigationView->pushView(secondButton, "Second Page");
};

navigationView->pushView(firstButton, "First Page");

```

## View manipulation

* **void pushView(std::shared_ptr<[View](view.md)\> view, [String](../foundation/string.md) title)**

	Pushes the given view with the specified title to the top of the stack. 

* **void popView()**

	Pops the topmost view from the stack.

## Relationships

Inherits from [`View`](view.md).

