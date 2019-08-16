path: tree/master/framework/ui/include/bdn/ui
source: ContainerView.h

# ContainerView

A view that can hold child views. 

## Declaration

```C++
namespace bdn::ui {
	class ContainerView : public View
}
```

## Example

```C++
#include <bdn/ui/ContainerView.h>
#include <bdn/ui/TextView.h>
#include <bdn/ui/Button.h>
// ...
auto container = std::make_shared<ContainerView>();

auto label = std::make_shared<TextView>();
auto button = std::make_shared<Button>();

container->addChildView(label);
container->addChildView(button);
```

## Managing Child Views

* **void addChildView(const std::shared_ptr<[View](view.md)\> &childView)**

	Adds a new child view to the container view.

* **void removeChildView(const std::shared_ptr<[View](view.md)\> &childView)**

	Removes the given child view from the container view.

* **void removeAllChildViews()**

	Removes all child views from the container view.

* **std::optional<std::vector::size_type\> childIndex(const std::shared_ptr<View\>& child)**

	Returns the index of `child` inside the list of child views or `std::nullopt` if
	the child cannot be found.


## Relationships

Inherits from: [View](view.md)
 