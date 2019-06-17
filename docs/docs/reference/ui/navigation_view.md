path: tree/master/framework/ui/include/bdn/ui
source: NavigationView.h

# NavigationView

Provides the user with a way to navigate between a number of different views where only one view is visible at a time.

## Declaration

```C++
namespace bdn::ui {
	class NavigationView : public View
}
```

## Reacting to Changes

The NavigationView signals the state of a view via its [`visible`](view.md#properties) property.

When a new view is pushed, its [`visible`](view.md#properties) property is set to `true`,
while the previously visible view's [`visible`](view.md#properties) property is set to `false`. The same
happens if e.g. the back button is clicked by the user.

## Example

```C++
#include <bdn/ui/Button.h>
#include <bdn/ui/NavigationView.h>
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

